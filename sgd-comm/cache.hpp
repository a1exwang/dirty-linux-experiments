#pragma once
#include <string>
#include <vector>
#include <map>
#include <curl/curl.h>
#include <functional>
#include <iostream>
#include <cstring>

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
      int http_worker_count)
      :server_list(server_list),
       on_miss_(server_list, http_worker_count),
       http_worker_count(http_worker_count),
       buffer_size_(1048576*1024),
       buffer_(new int8_t[1048576 * 1024]),
       current_offset(0)
       {}

  IOCache(const IOCache &rhs) = delete;
  IOCache(IOCache &&rhs) = delete;
  IOCache &operator=(const IOCache &rhs) = delete;
  IOCache &operator=(IOCache &&rhs) = delete;

  bool Exists(const std::string &key) const {
    return hash_data_.find(key) != hash_data_.end();
  }

  void GetOrRetrieve(const std::string &key, int8_t const* *buf, int64_t *size) {
    if (Exists(key)) {
      int64_t offset;
      std::tie(offset, *size) = hash_data_[key];
      *buf = this->buffer_ + offset;
      return;
    }
    std::cerr << "cache miss on '" << key << "'" << std::endl;
    int64_t status_code = 0;

    std::vector<char> data;
    on_miss_.Perform(key, data, status_code);
    if (current_offset + data.size() > buffer_size_) {
      // TODO: error reporting
      throw "buffer overflow";
    }

    int8_t *current_buf = this->buffer_+current_offset;
    memcpy(current_buf, data.data(), data.size());
    hash_data_[key] = std::make_pair(current_offset, data.size());

    current_offset += data.size();
    *buf = current_buf;
    *size = data.size();
  }
  int8_t *buffer() { return buffer_; }
  int64_t buffer_size() { return buffer_size_; }
  std::pair<int64_t, int64_t> find(const std::string &key) { return hash_data_[key]; }
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
static inline int64_t DefaultDistributionHash(int64_t _seed, int64_t image_id, int64_t processes_per_node, int64_t nodes, int64_t images_per_process, int64_t total_images) {
  int64_t total_cached_imgaes = processes_per_node * nodes * images_per_process;
  int64_t total_process = processes_per_node * nodes;
  if (total_cached_imgaes >= total_images) {
    return int64_t(image_id / (double(total_images) / total_process));
  } else {
    return int64_t(image_id / (double(total_cached_imgaes) / total_process));
  }
}

}
