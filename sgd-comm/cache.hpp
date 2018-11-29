#pragma once
#include <string>
#include <vector>
#include <map>
#include <curl/curl.h>
#include <functional>
#include <iostream>
#include <cstring>
#include <mpi.h>

namespace caffe {

inline static size_t write_body(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  auto builder = (std::vector<char>*)userp;

  char *p = (char*)contents;
  for (int i = 0; i < realsize; ++i) {
    builder->push_back(p[i]);
  }
  return realsize;
}

class RetrieveFromSeaweedFS {
public:
  RetrieveFromSeaweedFS(const std::vector<std::string> &server_list, int http_worker_count)
  :seaweedfs_masters_(server_list), http_worker_count(http_worker_count){
    curl_ = curl_easy_init();
  }
  ~RetrieveFromSeaweedFS() {
    if (curl_)
      curl_easy_cleanup(curl_);
  }
  void Perform(const std::string &key, std::vector<char> &buf, int status_code) {
    size_t r = (this->rr_counter_++) % this->seaweedfs_masters_.size();
    auto master = this->seaweedfs_masters_[r];
    auto url = master + "/" + key;

    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl_, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_body);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &buf);

    double elapsed;
    auto res = curl_easy_perform(curl_);
    if (res == CURLE_OK) {
      curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &status_code);
      curl_easy_getinfo(curl_, CURLINFO_TOTAL_TIME, &elapsed);
    } else {
      std::cerr << "curl_easy_perform() failed: err='" << curl_easy_strerror(res) << "', SeaweedFS URL: " << url;
    }
  }
private:
  CURL *curl_; int64_t rr_counter_;
  std::vector<std::string> seaweedfs_masters_;
  int http_worker_count;
};

template<typename Miss>
class IOCache {
public:
  IOCache(
      std::vector<std::string> server_list,
      int http_worker_count,
      int64_t buffer_size)
      :server_list(server_list),
       on_miss_(server_list, http_worker_count),
       http_worker_count(http_worker_count),
       buffer_size_(buffer_size),
       buffer_(new int8_t[buffer_size]),
       current_offset(0)
       {}
  ~IOCache() { delete []buffer_; }
  IOCache(const IOCache &rhs) = delete;
  IOCache(IOCache &&rhs) = delete;
  IOCache &operator=(const IOCache &rhs) = delete;
  IOCache &operator=(IOCache &&rhs) = delete;

  bool Exists(const std::string &key) const {
    return hash_data_.find(key) != hash_data_.end();
  }

  /**
   * Get from local cache or retrieve from remote file server.
   * If local cache is full or cache_placement == false,
   *   local cache is not filled after miss, even if remote_data is correctly fetched.
   * @param key: file key
   * @param buf: [OUT] result data
   * @param size: [OUT] result size
   * @param cache_placement: if true, place miss result in local cache
   * @return local cache offset
   */
  int64_t GetOrRetrieve(const std::string &key, std::vector<char> &data, bool cache_placement = true) {
    if (Exists(key)) {
      int64_t offset;
      int64_t size;
      std::tie(offset, size) = hash_data_[key];
      data.resize(size_t(size));
      memcpy(data.data(), this->buffer_+offset, size);
      return offset;
    }
//    std::cerr << "cache miss on '" << key << "'" << std::endl;
    int64_t status_code = 0;

    on_miss_.Perform(key, data, status_code);
//    printf("perform: data size %ld\n", data.size());

    int64_t ret = current_offset;
    int8_t *current_buf = this->buffer_+current_offset;

    if (!cache_placement || current_offset + data.size() > buffer_size_) {
      return -1;
    }

    // place in local cache
    memcpy(current_buf, data.data(), data.size());
    hash_data_[key] = std::make_pair(current_offset, data.size());
    current_offset += data.size();
    return ret;
  }
  int8_t *buffer() { return buffer_; }
  int64_t buffer_size() { return buffer_size_; }
private:
  std::vector<std::string> server_list;
  int64_t http_worker_count;

  Miss on_miss_;
  std::map<std::string, std::pair<int64_t, int64_t>> hash_data_;
  int8_t *buffer_;
  int64_t current_offset;
  int64_t buffer_size_;
};

typedef std::function<int64_t(int64_t seed, int64_t image_id, int64_t processes_per_node, int64_t nodes, int64_t total)> DistributionHash;
static inline int64_t DefaultDistributionHash(int64_t _seed, int64_t image_id, int64_t processes_per_node, int64_t nodes, int64_t cached_images_per_process, int64_t total_images) {
  int64_t total_processes = processes_per_node * nodes;
  int64_t total_cached_images = total_processes * cached_images_per_process;

  if (image_id >= total_cached_images) {
    return -1;
  } else {
    return image_id % total_processes;
  }
}

typedef caffe::IOCache<caffe::RetrieveFromSeaweedFS> MyCache;

enum CacheHitType {
  Local,
  Cluster,
  Miss
};
class DistributedCache {
public:
  DistributedCache(const std::vector<std::string> &server_list, int64_t http_worker_count, int64_t buffer_size,
      int64_t image_count, int rank, std::function <int (int64_t)> f)
    :rank(rank), f(std::move(f)), cache(server_list, http_worker_count, buffer_size),
     image_count(image_count), global_index_((int64_t*)calloc(image_count*2, sizeof(int64_t))) {
  }
  ~DistributedCache() {
    free(this->global_index_);
  }
  std::tuple<const int8_t*, int64_t> Get(
      const std::vector<std::string> &file_list,
      int64_t image_id,
      bool fill_global_index,
      bool use_cluster_cache,
      CacheHitType *hitType = nullptr) {
    int hash_rank = f(image_id);
    int8_t *image_data = nullptr;
    if (hash_rank == rank) {
      // local cache hit
      if (hitType != nullptr) {
        *hitType = CacheHitType::Local;
      }
      std::vector<char> data;
      int64_t offset = cache.GetOrRetrieve(file_list[image_id], data);
//      printf("cache.GetOrRetrieve: (offset, size) = (%ld, %ld)\n", offset, data.size());

      if (fill_global_index) {
        this->set(image_id, std::make_tuple(offset, data.size()));
      }

      image_data = new int8_t[data.size()];
      memcpy(image_data, data.data(), (size_t) data.size());
      return std::make_tuple(image_data, data.size());
    } else if (!use_cluster_cache || hash_rank < 0) {
      if (hitType != nullptr) {
        *hitType = CacheHitType::Miss;
      }
      return this->cacheMiss(file_list[image_id]);
    } else {
      // cluster cache hit and use_cluster_cache
      if (hitType != nullptr) {
        *hitType = CacheHitType::Cluster;
      }
      return clusterCacheHit(image_id, hash_rank);
    }
  }

  void Sync() {
    MPI_Allreduce(MPI_IN_PLACE, this->global_index_, int(image_count*2), MPI_INT64_T, MPI_SUM, MPI_COMM_WORLD);
    MPI_Win_create(cache.buffer(), cache.buffer_size(), sizeof(int8_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    MPI_Win_fence(0, win);
  }

  std::tuple<const int8_t*, int64_t> cacheMiss(const std::string &file_name) {
    std::vector<char> data;
    cache.GetOrRetrieve(file_name, data, false);
    auto image_data = new int8_t[data.size()];
    memcpy(image_data, data.data(), data.size());
    return std::make_tuple(image_data, data.size());
  }

  std::tuple<const int8_t*, int64_t> clusterCacheHit(int64_t image_id, int target_rank) {
    int64_t offset; int64_t buf_size;
    std::tie(offset, buf_size) = this->at(image_id);
    auto image_data = new int8_t[buf_size];

    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, target_rank, 0, win);
    MPI_Get(image_data, (int) buf_size, MPI_BYTE, target_rank, offset, (int) buf_size, MPI_BYTE, win);
    MPI_Win_unlock(target_rank, win);
    return std::make_tuple(image_data, buf_size);
  }

private:

  inline std::tuple<int64_t, int64_t> at(int64_t image_id) const {
    return std::make_tuple(this->global_index_[image_id*2 + 0], this->global_index_[image_id*2 + 1]);
  }
  inline void set(int64_t image_id, std::tuple<int64_t, int64_t> offset_size){
    std::tie(this->global_index_[image_id*2 + 0], this->global_index_[image_id*2 + 1]) = offset_size;
  }

  int64_t image_count;
  int64_t *global_index_;
private:
  MPI_Win win;
  MyCache cache;
  int rank;
  std::function <int (int64_t)> f;
};

};
