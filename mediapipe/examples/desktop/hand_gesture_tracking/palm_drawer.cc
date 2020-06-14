#include "palm_drawer.h"

namespace {
constexpr char kPalmWindowName[] = "Palm";
constexpr int kWindowsSize = 500;
const auto kWindowsBackground = cv::Scalar(60, 60, 60);
constexpr int kPalmPointRadius = 5;
const auto kPalm1PointColor = cv::Scalar(0, 200, 60);
const auto kPalm2PointColor = cv::Scalar(0, 60, 200);

inline cv::Point MakePoint(std::pair<float, float> p) {
  return {p.first * kWindowsSize, p.second * kWindowsSize};
}
}  // namespace

PalmDrawer::PalmDrawer()
    : img{kWindowsSize, kWindowsSize, CV_8UC3, cv::Scalar::all(0)} {
  cv::namedWindow(kPalmWindowName);
  std::cout << "PalmDrawer created\r\n";
}
void PalmDrawer::SetPalmInfo(mediapipe::Timestamp t, NormalizedPointsVec _palm1,
                             NormalizedPointsVec _palm2) {
  std::lock_guard<std::mutex> lock(mutex);
  if (lastTimestamp > t) return;

  std::cout << "PalmDrawer::SetPalmInfo\r\n";
  lastTimestamp = t;
  palm1 = std::move(_palm1);
  palm2 = std::move(_palm2);
}
void PalmDrawer::Show() {  // call only from main thread
  std::lock_guard<std::mutex> lock(mutex);
  std::cout << "PalmDrawer::Show\r\n";
  UpdateImage();
  cv::imshow(kPalmWindowName, img);
}
PalmDrawer& PalmDrawer::GetInst() {
  static PalmDrawer inst;
  return inst;
}
void PalmDrawer::UpdateImage() {
  img.setTo(kWindowsBackground);

  std::cout << "PalmDrawer::Show\r\n";
  std::cout << "palm1: " << palm1.size() << "\r\n";
  for (size_t i = 0; i < palm1.size(); ++i) {
    cv::Point p = MakePoint(palm1[i]);
    std::cout << i << ": (" << p.x << ":" << p.y << "\r\n";
    cv::circle(img, MakePoint(palm1[i]), kPalmPointRadius, kPalm1PointColor);
  }
  std::cout << "palm2: " << palm2.size() << "\r\n";
  for (size_t i = 0; i < palm2.size(); ++i) {
    cv::circle(img, MakePoint(palm2[i]), kPalmPointRadius, kPalm2PointColor);
  }
}