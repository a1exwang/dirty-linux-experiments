#include "cache.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <opencv2/imgcodecs.hpp>

using namespace std;

typedef caffe::IOCache<caffe::RetrieveFromSeaweedFS> MyCache;

MyCache *create_cache(vector<string> &file_list) {
  vector<string> server_list = {"127.0.0.1:8083"};
  int64_t worker_count = 8;

  string line;
  ifstream ifs("idlist.train");
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

void use_data(const std::string &name, const vector<char> &data) {
  cv::Mat img = cv::imdecode(cv::Mat(data), 0);
  assert(img.data);
  cout << name << " " << img.rows << "*" << img.cols << endl;
}
int main() {
  vector<string> file_list;
  auto cache = create_cache(file_list);

  for (int64_t i = 0; i < file_list.size(); i++) {
    const int8_t *buf; int64_t size;
    cache->GetOrRetrieve(file_list[i], &buf, &size);
    vector<char> current_data((char*)buf, (char*)buf + size);
    use_data(file_list[i], current_data);
  }
}
