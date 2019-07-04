#include <map>
#include <queue>
#include <map>
#include <string>


class Context{
 public:
  bool image_flip = false;
  bool image_rotate90_after_flip = false;
  cv::Mat_<float> cam_intrisic;
  cv::Mat_<float> cam_distCoffe;

  std::queue<std::string> image_list_queue;
  std::mutex image_list_queue_lock;

  bool is_process_stop = false;
  std::map<std::string, Detection_Result> detection_result;

  bool enable_detect_face;
  std::string model_path;

  bool enable_detect_rt;
  int chessboard_height = 5;
  int chessboard_wight = 6;
  float chessboard_interval = 0.01;
  bool is_three_board = false;
  cv::Mat_<float> bd_01_r;
  cv::Mat_<float> bd_01_t;
  cv::Mat_<float> bd_21_r;
  cv::Mat_<float> bd_21_t;
};

int main() {
  return 0;
}