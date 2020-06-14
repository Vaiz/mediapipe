#include <mutex>
#include <opencv2/opencv.hpp>
#include "mediapipe/framework/timestamp.h"

class PalmDrawer {
  using NormalizedPointsVec = std::vector<std::pair<float, float>>;

 public:
  PalmDrawer();
  void SetPalmInfo(mediapipe::Timestamp t, NormalizedPointsVec palm1,
                   NormalizedPointsVec palm2);
  void Show();  // call only from main thread'
  static PalmDrawer& GetInst();

 private:
  void UpdateImage();

 private:
  std::mutex mutex;
  mediapipe::Timestamp lastTimestamp {0};
  cv::Mat img;
  NormalizedPointsVec palm1;
  NormalizedPointsVec palm2;
};