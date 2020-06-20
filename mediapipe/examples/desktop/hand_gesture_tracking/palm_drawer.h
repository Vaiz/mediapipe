#pragma once

#include <mutex>
#include <opencv2/opencv.hpp>

#include "mediapipe/framework/timestamp.h"
#include "palm.h"

class PalmDrawer {
  using NormalizedPointsVec = std::vector<cv::Point2f>;

 public:
  PalmDrawer();
  void SetPalmInfo(mediapipe::Timestamp t, Palm palm1, Palm palm2);
  void Show();  // call only from main thread'
  static PalmDrawer& GetInst();

 private:
  void UpdateImage();
  void DrawPalm(const Palm& palm, cv::Scalar color);

 private:
  std::mutex mutex;
  mediapipe::Timestamp lastTimestamp{0};
  cv::Mat img;
  Palm palm1;
  Palm palm2;
};