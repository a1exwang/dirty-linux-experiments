#include "mpi.h"
#include "stdio.h"
#include <thread>
#include "cache.hpp"
#include <fstream>
#include <opencv2/opencv.hpp>


// Build with
// g++ -o mpi_win -std=c++11 mpi_win.cpp -I/usr/local/openmpi-3.0.0/include/ -I/usr/local/include /usr/local/openmpi-3.0.0/lib/libmpi.so /usr/local/lib/libopencv_core.so /usr/local/lib/libopencv_highgui.so -lcurl

using namespace std;

typedef caffe::IOCache<caffe::RetrieveFromSeaweedFS> MyCache;

MyCache *create_cache(vector<string> &file_list, const vector<string> &server_list, string filename) {
  int64_t worker_count = 8;

  string line;
  ifstream ifs(filename);
  while (!ifs.eof()) {
    getline(ifs, line);
    if (line.size() > 0) {
      auto off = line.find(' ');
      auto file_path = line.substr(0, off);
      file_list.push_back(file_path);
      line.substr(off+1, line.size() - off);
    }
  }

  int64_t buffer_size = file_list.size() * 200 * 1024;
  return new MyCache(server_list, worker_count, buffer_size);
}

void use_data(const std::string &name, const int8_t *data, int64_t size) {
  cv::Mat img = cv::imdecode(cv::Mat(vector<char>((char*)data, (char*)data+size)), 0);
  assert(img.data);
//  cout << name << " " << img.rows << "*" << img.cols << endl;
}

int main(int argc, char *argv[]) {
  int rank, nprocs;
  MPI_Win win;

  const char *filename = argv[1];
  string masters(argv[2]);
  int nodes = atoi(argv[3]);
  int images_per_proc = atoi(argv[4]);

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

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int64_t print_size = 1000;
  // assume 200K per image

  vector<string> file_list;
  auto cache = create_cache(file_list, server_list, filename);

  // epoch 0
  int64_t buf_size_total = 0;
  auto t0 = std::chrono::high_resolution_clock::now();
  for (int64_t i = 0; i < file_list.size(); i++) {
    const int8_t *buf;
    int64_t buf_size;
    cache->GetOrRetrieve(file_list[i], &buf, &buf_size);
    buf_size_total += buf_size;
    use_data(file_list[i], buf, buf_size);
    if (i % print_size == 0 && rank == 0) {
      auto t1 = std::chrono::high_resolution_clock::now();
      auto deltat = (t1-t0).count()/1e9;
      double iops = 1/deltat * print_size;
      double tp = buf_size_total / deltat / 1024/1024;
      MPI_Allreduce(MPI_IN_PLACE, &iops, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
      MPI_Allreduce(MPI_IN_PLACE, &tp, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
      printf("epoch 0, rank %d time: %fs, total IOPS: %f, tp=%.2fMiB/s, i: %ld\n", rank, deltat, iops, tp, i);
      t0 = t1;
      buf_size_total = 0;
    }
  }
  // TODO: MPI Scatter to let everybody know the file_key -> <offset, len> hash table

  printf("Epoch 0 done, starting other epochs, rank=%d\n", rank);
  MPI_Barrier(MPI_COMM_WORLD);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  MPI_Win_create(cache->buffer(), cache->buffer_size(), sizeof(int8_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
  MPI_Win_fence(0, win);

  for (int64_t epoch = 1; epoch < 10; epoch++) {
    buf_size_total = 0;
    auto epoch_start = std::chrono::high_resolution_clock::now();
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int64_t i = 0; i < file_list.size(); i++) {
      int r = (int)caffe::DefaultDistributionHash(0, i,
          nprocs/nodes, nodes, images_per_proc, file_list.size());
      if (rank == r) {
        const int8_t *buf; int64_t buf_size;
        cache->GetOrRetrieve(file_list[i], &buf, &buf_size);
        use_data(file_list[i], buf, buf_size);
      } else {
        int64 offset, buf_size;
        tie(offset, buf_size) = cache->find(file_list[i]);
        auto remote_data = new int8_t[buf_size];

        // TODO: no need to do this
        int64_t real_size; const int8_t *real_buf;
        cache->GetOrRetrieve(file_list[i], &real_buf, &real_size);

        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, r, 0, win);
        MPI_Get(remote_data, buf_size, MPI_BYTE, r, offset, buf_size, MPI_BYTE, win);
        MPI_Win_unlock(r, win);

        printf("Access remote cache, rank=%d, remote_rank=%d, remote_offset=%ld, remote_size=%ld, valid=%d\n",
               rank, r, offset, buf_size, memcmp(remote_data, real_buf, buf_size) == 0);
      }
      if (i % print_size == 0 && rank == 0) {
        auto t1 = std::chrono::high_resolution_clock::now();
        auto deltat = (t1-t0).count()/1e9;
        double iops = 1/deltat * print_size;
        double tp = buf_size_total / deltat / 1024/1024;
        MPI_Allreduce(MPI_IN_PLACE, &iops, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(MPI_IN_PLACE, &tp, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        printf("epoch 0, rank %d time: %fs, total IOPS: %f, tp=%.2f, i: %ld\n", rank, deltat, iops, tp, i);
        t0 = t1;
        buf_size_total = 0;
      }
    }
    printf("epoch %ld time: %fs\n", epoch, (std::chrono::high_resolution_clock::now()-epoch_start).count()/1e9);
  }

  MPI_Win_free(&win);
  MPI_Finalize();
  return 0;
}
