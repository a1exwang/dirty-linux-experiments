#pragma once
#include <string>
#include <vector>
#include <map>
#include <curl/curl.h>
#include <functional>
#include <iostream>
#include <cstring>
#include <random>
#include <set>
#include <mpi.h>
#include <glog/logging.h>


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
      :seaweedfs_masters_(server_list), http_worker_count(http_worker_count) {
    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if (res != 0) {
      LOG(ERROR) << "Failed global init ..." << std::endl;
      abort();
    }
  }
  ~RetrieveFromSeaweedFS() {
  }
  void Perform(const std::string &key, std::vector<char> &buf, int &status_code) {
    size_t r = (this->rr_counter_++) % this->seaweedfs_masters_.size();
    auto master = this->seaweedfs_masters_[r];
    auto url = master + "/" + key;

    auto curl = curl_easy_init();
    if (!curl) {
      LOG(ERROR) << "curl_easy_init() failed" << std::endl;
      status_code = 0;
      return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_body);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);

    double elapsed;
    auto res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
      curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
    } else {
      LOG(ERROR) << "curl_easy_perform() failed: err='" << curl_easy_strerror(res) << "', SeaweedFS URL: " << url;
    }

    if (curl)
      curl_easy_cleanup(curl);
  }
private:
  CURL *curl_;
  int64_t rr_counter_;
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
       http_worker_count(http_worker_count),
       on_miss_(server_list, http_worker_count),
       buffer_(new int8_t[buffer_size]),
       current_offset(0),
       buffer_size_(buffer_size) {}

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
  int64_t GetOrRetrieve(const std::string &key,
                        std::vector<char> &data,
                        bool cache_placement = true,
                        bool *ok = nullptr,
                        bool *hit = nullptr
  ) {
    if (hit)
      *hit = false;

    if (Exists(key)) {
      if (hit)
        *hit = true;
      int64_t offset;
      int64_t size;
      std::tie(offset, size) = hash_data_[key];
      data.resize(size_t(size));
      memcpy(data.data(), this->buffer_+offset, size);
      if (ok) {
        *ok = true;
      }
      return offset;
    }
//    std::cerr << "cache miss on '" << key << "'" << std::endl;
    int status_code = 0;

    on_miss_.Perform(key, data, status_code);
    if (!(status_code >= 200 && status_code < 300)) {
      if (ok) {
        *ok = false;
      }
      return -1;
    }
//    printf("perform: data size %ld\n", data.size());

    if (ok) {
      *ok = true;
    }

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

typedef std::function<std::set<int> (int64_t seed, int64_t image_id, int64_t processes_per_node, int64_t nodes, int64_t total)> DistributionHash;
static inline std::set<int> DefaultDistributionHash(int64_t _seed, int64_t image_id, int64_t processes_per_node, int64_t nodes, int64_t cached_images_per_process, int64_t total_images) {
  int64_t total_processes = processes_per_node * nodes;
  int64_t total_cached_images = total_processes * cached_images_per_process;
  if (image_id >= total_cached_images) {
    return {};
  } else {
    std::set<int> ret;

    if (total_images == 0) return {};
    auto more_replicated_images = total_cached_images % total_images;
    auto replication_count = (int64_t)floor(double(total_cached_images) / total_images);
    int64_t start = 0;
    if (image_id >= more_replicated_images) {
      start = more_replicated_images*(replication_count+1) + (image_id-more_replicated_images)*replication_count;
      for (int64_t i = 0; i < replication_count; i++) {
        ret.insert(int((start+i) % total_processes));
      }
    } else {
      start = image_id * (replication_count+1);
      for (int64_t i = 0; i < replication_count+1; i++) {
        ret.insert(int((start+i) % total_processes));
      }
    }
    return ret;
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
                   int64_t image_count, int total_procs, int rank, double cluster_yield_rate, std::function <std::set<int> (int64_t)> f)
      :image_count(image_count),
       global_index_((int64_t*)calloc(image_count*2*total_procs, sizeof(int64_t))),
       win(),
       cache(server_list, http_worker_count, buffer_size),
       rank(rank),
       total_procs(total_procs),
       f(std::move(f)),
       cluster_yield_rate(cluster_yield_rate),
       generator(),
       distribution(0, 1) {}

  ~DistributedCache() {
    free(this->global_index_);
  }

  std::tuple<const int8_t*, int64_t> Get(
      const std::string &file_name,
      int64_t image_id,
      bool fill_global_index,
      bool use_cluster_cache,
      bool disable_cache,
      CacheHitType *hit_type = nullptr) {

    if (disable_cache) {
      if (hit_type)
        *hit_type = CacheHitType::Miss;
      return cacheMiss(file_name);
    }

    std::set<int> target_ranks = f(image_id);
    int8_t *image_data = nullptr;
    if (target_ranks.find(rank) != target_ranks.end()) {
      // local cache hit
      std::vector<char> data;
      bool ok, hit;
      int64_t offset = cache.GetOrRetrieve(file_name, data, true, &ok, &hit);
      if (!ok) {
        return std::make_tuple(nullptr, 0);
      }
      *hit_type = hit ? CacheHitType::Local : CacheHitType::Miss;

      if (fill_global_index) {
        this->set(image_id, rank, std::make_tuple(offset, data.size()));
      }

      image_data = new int8_t[data.size()];
      memcpy(image_data, data.data(), (size_t) data.size());
      return std::make_tuple(image_data, data.size());
    } else if (target_ranks.size() == 0 || !use_cluster_cache || should_yield_cluster_cache()) {
      if (hit_type != nullptr) {
        *hit_type = CacheHitType::Miss;
      }
      return this->cacheMiss(file_name);
    } else {
      // may be cluster cache hit and use_cluster_cache
      const int8_t *buf; int64_t buf_size;
      // TODO: better choice algorithm
      auto target_rank_chosen = *target_ranks.begin();
      std::tie(buf, buf_size) = clusterCacheHit(image_id, target_rank_chosen);
      if (buf == nullptr) {
        if (hit_type != nullptr) {
          *hit_type = CacheHitType::Miss;
        }
        return this->cacheMiss(file_name);
      } else {
        if (hit_type != nullptr) {
          *hit_type = CacheHitType::Cluster;
        }
        return std::make_tuple(buf, buf_size);
      }
    }
  }

  void Sync() {
    LOG(ERROR) << "Epoch 0 done, start syncing cache total_size: "  << int(image_count*2*total_procs);
    MPI_Allreduce(MPI_IN_PLACE, this->global_index_, int(image_count*2*total_procs), MPI_INT64_T, MPI_SUM, MPI_COMM_WORLD);
    LOG(ERROR) << "Epoch 0 done, allreduce done";
    MPI_Win_create(cache.buffer(), cache.buffer_size(), sizeof(int8_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    LOG(ERROR) << "Epoch 0 done, win create done";
    MPI_Win_fence(0, win);
    LOG(ERROR) << "Epoch 0 done, fence done";
  }

  std::tuple<const int8_t*, int64_t> cacheMiss(const std::string &file_name) {
    bool ok;
    std::vector<char> data;
    cache.GetOrRetrieve(file_name, data, false, &ok);
    if (!ok) {
      return std::make_tuple(nullptr, 0);
    }
    auto image_data = new int8_t[data.size()];
    memcpy(image_data, data.data(), data.size());
    return std::make_tuple(image_data, data.size());
  }

  std::tuple<const int8_t*, int64_t> clusterCacheHit(int64_t image_id, int target_rank) {
    int64_t offset; int64_t buf_size;
    std::tie(offset, buf_size) = this->at(image_id, target_rank);
    if (offset < 0) {
      // cluster cache full, failed to retrieve from cluster cache.
      return std::make_tuple(nullptr, 0);
    }

    auto image_data = new int8_t[buf_size];
    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, target_rank, 0, win);
    MPI_Get(image_data, (int) buf_size, MPI_BYTE, target_rank, offset, (int) buf_size, MPI_BYTE, win);
    MPI_Win_unlock(target_rank, win);
    return std::make_tuple(image_data, buf_size);
  }

  bool should_yield_cluster_cache() {
    if (this->cluster_yield_rate == 0) {
      return false;
    } else if (this->cluster_yield_rate == 1) {
      return true;
    } else {
      double r = distribution(generator);
      return r <= this->cluster_yield_rate;
    }
  }

private:

  inline std::tuple<int64_t, int64_t> at(int64_t image_id, int target_rank) const {
    return std::make_tuple(
        this->global_index_[target_rank*image_count*2 + image_id*2 + 0],
        this->global_index_[target_rank*image_count*2 + image_id*2 + 1]
    );
  }
  inline void set(int64_t image_id, int target_rank, std::tuple<int64_t, int64_t> offset_size){
    std::tie(
        this->global_index_[target_rank*image_count*2 + image_id*2 + 0],
        this->global_index_[target_rank*image_count*2 + image_id*2 + 1]) = offset_size;
  }

  int64_t image_count;
  int64_t *global_index_;
  MPI_Win win;
  MyCache cache;
  int rank, total_procs;
  std::function <std::set<int> (int64_t)> f;
  double cluster_yield_rate;
  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution;
  int hits[3];
};
};
