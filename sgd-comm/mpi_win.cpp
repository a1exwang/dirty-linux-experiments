#include "mpi.h"
#include "stdio.h"
#include <thread>
#include "cache.hpp"
#include <fstream>
#include <opencv2/imgcodecs.hpp>


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

  return new MyCache(server_list, worker_count);
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
  if (nprocs != 2) {
    printf("Run this program with 2 processes\n");
    fflush(stdout);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  int other_rank = 0;
  if (rank == 0)
    other_rank = 1;

  vector<string> file_list;
  auto cache = create_cache(file_list, server_list, filename);

  // epoch 0
  auto t0 = std::chrono::high_resolution_clock::now();
  for (int64_t i = 0; i < file_list.size(); i++) {
    const int8_t *buf; int64_t buf_size;
    cache->GetOrRetrieve(file_list[i], &buf, &buf_size);
    use_data(file_list[i], buf, buf_size);
  }
  printf("epoch 0 time: %fs\n", (std::chrono::high_resolution_clock::now()-t0).count()/1e9);
  // TODO: MPI Scatter to let everybody know the file_key -> <offset, len> hash table

//  if (rank == 0) {
//    char *data = new char[size];
//    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, other_rank, 0, win);
//    int offset = 5;
//    MPI_Get(data, 1, MPI_CHAR, other_rank, offset, size - offset, MPI_CHAR, win);
//    MPI_Win_unlock(other_rank, win);
//    printf("data: '%s'\n", data);
//  }

  printf("Epoch 0 done, starting other epochs\n");
  std::this_thread::sleep_for(std::chrono::seconds(1));
  MPI_Win_create(cache->buffer(), cache->buffer_size(), sizeof(int8_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
  MPI_Win_fence(0, win);

  for (int64_t epoch = 1; epoch < 10; epoch++) {
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int64_t i = 0; i < file_list.size(); i++) {
      auto r = caffe::DefaultDistributionHash(0, i, 2, 1, file_list.size()/2, file_list.size());
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

        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, other_rank, 0, win);
        MPI_Get(remote_data, buf_size, MPI_BYTE, other_rank, offset, buf_size, MPI_BYTE, win);
        MPI_Win_unlock(other_rank, win);

//        printf("Access remote cache, rank=%d, remote_rank=%d, remote_offset=%ld, remote_size=%ld, valid=%d\n",
//               rank, other_rank, offset, buf_size, memcmp(remote_data, real_buf, buf_size) == 0);
      }
    }
    printf("epoch %ld time: %fs\n", epoch, (std::chrono::high_resolution_clock::now()-t0).count()/1e9);
  }

  MPI_Win_free(&win);
  MPI_Finalize();
  return 0;
}
