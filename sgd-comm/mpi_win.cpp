#include "mpi.h"
#include "stdio.h"
#include <thread>
#include "cache.hpp"
#include <fstream>
#include <opencv2/opencv.hpp>


// Build with
// g++ -o mpi_win -std=c++11 mpi_win.cpp -I/usr/local/openmpi-3.0.0/include/ -I/usr/local/include /usr/local/openmpi-3.0.0/lib/libmpi.so /usr/local/lib/libopencv_core.so /usr/local/lib/libopencv_highgui.so -lcurl

using namespace std;


void parse_file_list(vector<string> &file_list, const string &filename) {
  string line;
  ifstream ifs(filename);
  while (!ifs.eof()) {
    getline(ifs, line);
    if (!line.empty()) {
      auto off = line.find(' ');
      auto file_path = line.substr(0, off);
      file_list.push_back(file_path);
      line.substr(off+1, line.size() - off);
    }
  }
}

void use_data(const std::string &name, const int8_t *data, int64_t size) {
  cv::Mat img = cv::imdecode(cv::Mat(vector<char>((char*)data, (char*)data+size)), 0);
  assert(img.data);
}

void epoch_loop(const std::vector<string> &image_list, const function<int(int64_t)> &f, int rank,
                int64_t print_size, int64_t epoch,
                const function<int64_t (int64_t image_id)> &process_image) {
  static double epoch0_iops = 1;
  int64_t total_size = 0;
  auto epoch_start = std::chrono::high_resolution_clock::now();
  auto t0 = std::chrono::high_resolution_clock::now();
  for (int64_t image_id = 0; image_id < image_list.size(); image_id++) {
//    printf("image_id = %ld\n", image_id);

    auto size = process_image(image_id);

    total_size += size;

    if (image_id % print_size == 0) {
      auto t1 = std::chrono::high_resolution_clock::now();
      auto deltat = (t1-t0).count()/1e9;
      double iops = 1/deltat * print_size;
      double tp = total_size / deltat / 1024/1024;
      MPI_Allreduce(MPI_IN_PLACE, &iops, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
      MPI_Allreduce(MPI_IN_PLACE, &tp, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
      if (rank == 0) {
        printf("epoch=%ld time=%fs IOPS=%f tp=%.2fMiB/s i=%ld\n", epoch, deltat, iops, tp, image_id);
      }
      t0 = std::chrono::high_resolution_clock::now();
      total_size = 0;
    }
  }

  double epoch_time = (std::chrono::high_resolution_clock::now()-epoch_start).count()/1e9;
  double epoch_iops = image_list.size() / epoch_time;
  MPI_Allreduce(MPI_IN_PLACE, &epoch_iops, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  if (epoch == 0) {
    epoch0_iops = epoch_iops;
  }
  if (rank == 0) {
    printf("epoch %ld done, epoch_time=%.2f, epoch_iops=%0.2f, acceleration=%.2fx\n", epoch, epoch_time, epoch_iops, epoch_iops/epoch0_iops);
  }
}


int main(int argc, char *argv[]) {
  int rank, nprocs;
  const char *filename = argv[1];
  string masters(argv[2]);
  int64_t nodes = strtol(argv[3], nullptr, 10);
  int64_t images_per_proc = strtol(argv[4], nullptr, 10);
  int64_t do_decode = strtol(argv[5], nullptr, 10);
  int64_t print_size = strtol(argv[6], nullptr, 10);

  // parse server list
  vector<string> server_list;
  stringstream server;
  for (auto c : masters) {
    if (c == ',') {
      server_list.push_back(server.str());
      server.str(""); server.clear();
    }  else {
      server << c;
    }
  }
  if (!server.str().empty()) {
    server_list.push_back(server.str());
  }

  // parse file list
  vector<string> file_list;
  parse_file_list(file_list, filename);

  // init mpi
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // init cache
  int64_t buffer_size = file_list.size() * 200 * 1024;
  int64_t image_count = file_list.size();
  auto f = [=](int64_t image_id) -> int {
    return (int)caffe::DefaultDistributionHash(0, image_id, nprocs/nodes, nodes, images_per_proc, image_count);
  };
  caffe::DistributedCache distributedCache(server_list, 8, buffer_size, image_count, rank, f);

  // epoch 0
  epoch_loop(file_list, f, rank, print_size, 0, [&](int64_t image_id) -> int64_t {
    const int8_t *buf; int64_t buf_size;
    tie(buf, buf_size) = distributedCache.Get(file_list, image_id, true, false);
    if (do_decode)
      use_data(file_list[image_id], buf, buf_size);
    return buf_size;
  });

  // sync global index
  MPI_Barrier(MPI_COMM_WORLD);
  distributedCache.Sync();
  MPI_Barrier(MPI_COMM_WORLD);

  int64_t total_epochs = 10;
  caffe::CacheHitType hit_type;
  int64_t hits[3] = {0};
  for (int64_t epoch = 1; epoch < total_epochs; epoch++) {
    epoch_loop(file_list, f, rank, print_size, epoch, [&](int64_t image_id) -> int64_t {
      const int8_t *image_data = nullptr;
      int64_t buf_size = 0;

      // TODO: improve memory management
      tie(image_data, buf_size) = distributedCache.Get(file_list, image_id, false, true, &hit_type);
      hits[hit_type]++;

      if (do_decode)
        use_data(file_list[image_id], image_data, buf_size);
      delete []image_data;
      return buf_size;
    });

    double total = hits[0] + hits[1] + hits[2];
    printf("rank %d, cache hit rate: local %.2f%%, cluster %.2f%%, miss %.2f%%\n", rank, hits[0]/total*100, hits[1]/total*100, hits[2]/total*100);
  }

  MPI_Finalize();
  return 0;
}
