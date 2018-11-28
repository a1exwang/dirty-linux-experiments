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
}

class GlobalCache {
public:
  GlobalCache(int64_t image_count)
    :image_count(image_count),
      data_((int64_t*)calloc(image_count*2, sizeof(int64_t))) {
  }
  ~GlobalCache() {
    free(data_);
  }

  inline tuple<int64_t, int64_t> at(int64_t image_id) const {
    return make_tuple(this->data_[image_id*2 + 0], this->data_[image_id*2 + 1]);
  }
  inline void set(int64_t image_id, int64_t offset, int64_t buf_size){
    this->data_[image_id*2 + 0] = offset;
    this->data_[image_id*2 + 1] = buf_size;
  }

  void sync() {
    MPI_Allreduce(MPI_IN_PLACE, this->data_, int(image_count*2), MPI_INT64_T, MPI_SUM, MPI_COMM_WORLD);
  }
private:
  int64_t image_count;
  int64_t *data_;
};


int main(int argc, char *argv[]) {
  int rank, nprocs;
  MPI_Win win;

  const char *filename = argv[1];
  string masters(argv[2]);
  int nodes = atoi(argv[3]);
  int images_per_proc = atoi(argv[4]);
  int do_decode = atoi(argv[5]);
  int64_t print_size = atol(argv[6]);

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
  // assume 200K per image

  vector<string> file_list;
  auto cache = create_cache(file_list, server_list, filename);
  int64_t image_count = file_list.size();
  auto f = [=](int64_t image_id) -> int {
    return (int)caffe::DefaultDistributionHash(0, image_id, nprocs/nodes, nodes, images_per_proc, image_count);
  };

  // image_id -> (offset, len)
  GlobalCache global_index(image_count);

  // epoch 0
  int64_t buf_size_total = 0;
  auto epoch0_start = std::chrono::high_resolution_clock::now();
  auto t0 = std::chrono::high_resolution_clock::now();
  int64_t i_local = 0;
  for (int64_t image_id = 0; image_id < image_count; image_id++) {
    const int8_t *buf;
    int64_t buf_size;
    // only file buffer for local
    int r = f(image_id);
    if (r == rank) {
      i_local++;
      int64_t offset = cache->GetOrRetrieve(file_list[image_id], &buf, &buf_size);
      if (do_decode)
        use_data(file_list[image_id], buf, buf_size);

      global_index.set(image_id, offset, buf_size);

      buf_size_total += buf_size;
      if (i_local % print_size == 0) {
        auto t1 = std::chrono::high_resolution_clock::now();
        auto deltat = (t1-t0).count()/1e9;
        double iops = 1/deltat * print_size;
        double tp = buf_size_total / deltat / 1024/1024;
        MPI_Allreduce(MPI_IN_PLACE, &iops, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(MPI_IN_PLACE, &tp, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        if (rank == 0) {
          printf("epoch=0 time=%fs IOPS=%f tp=%.2fMiB/s i=%ld\n", deltat, iops, tp, i_local);
        }
        t0 = std::chrono::high_resolution_clock::now();
        buf_size_total = 0;
      }
    }
  }
  double epoch0_time = (std::chrono::high_resolution_clock::now()-epoch0_start).count()/1e9;
  double epoch0_iops = i_local / epoch0_time;
  MPI_Allreduce(MPI_IN_PLACE, &epoch0_iops, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  if (rank == 0) {
    printf("Epoch 0 done, syncing map\n");
  }
  global_index.sync();
  if (rank == 0) {
    printf("Epoch 0 done, starting other epochs, rank=%d\n", rank);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  MPI_Win_create(cache->buffer(), cache->buffer_size(), sizeof(int8_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
  MPI_Win_fence(0, win);

  int64_t total_epochs = 10;
  for (int64_t epoch = 1; epoch <= total_epochs; epoch++) {
    buf_size_total = 0;
    auto epoch_start = std::chrono::high_resolution_clock::now();
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int64_t image_id = 0; image_id < file_list.size(); image_id++) {
      int r = f(image_id);
      if (r == rank) {
        // local cache hit
        const int8_t *buf; int64_t buf_size;
        cache->GetOrRetrieve(file_list[image_id], &buf, &buf_size);
        if (do_decode)
          use_data(file_list[image_id], buf, buf_size);
        buf_size_total += buf_size;
      } else if (r < 0) {
        // no cache hit

      } else {
        // cluster cache hit
        int64 offset, buf_size;
        tie(offset, buf_size) = global_index.at(image_id);
        auto remote_data = new int8_t[buf_size];

        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, r, 0, win);
        MPI_Get(remote_data, buf_size, MPI_BYTE, r, offset, buf_size, MPI_BYTE, win);
        MPI_Win_unlock(r, win);

        if (do_decode)
          use_data(file_list[image_id], remote_data, buf_size);

        buf_size_total += buf_size;
        delete []remote_data;
      }
      if (image_id % print_size == 0) {
        auto t1 = std::chrono::high_resolution_clock::now();
        auto deltat = (t1-t0).count()/1e9;
        double iops = 1/deltat * print_size;
        double tp = buf_size_total / deltat / 1024/1024;
        MPI_Allreduce(MPI_IN_PLACE, &iops, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(MPI_IN_PLACE, &tp, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        if (rank == 0) {
          printf("epoch=%ld time=%fs IOPS=%.2f tp=%.2fMiB/s i=%ld\n", epoch, deltat, iops, tp, image_id);
        }
        t0 = std::chrono::high_resolution_clock::now();
        buf_size_total = 0;
      }
    }
    // Epoch barrier
    MPI_Barrier(MPI_COMM_WORLD);
    double epoch_time = (std::chrono::high_resolution_clock::now()-epoch_start).count()/1e9;
    double epoch_iops = image_count / epoch_time;
    MPI_Allreduce(MPI_IN_PLACE, &epoch_iops, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    if (rank == 0)
      printf("epoch %ld time: %fs, acceleration: %.2f times.\n", epoch, epoch_time, epoch_iops/epoch0_iops*nprocs);
  }

  MPI_Win_free(&win);
  MPI_Finalize();
  return 0;
}
