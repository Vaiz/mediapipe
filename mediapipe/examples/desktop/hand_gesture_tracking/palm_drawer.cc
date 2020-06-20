#include "palm_drawer.h"

namespace {
constexpr char kPalmWindowName[] = "Palm";
constexpr int kWindowsSize = 500;
const auto kWindowsBackground = cv::Scalar(60, 60, 60);
constexpr int kPalmPointRadius = 5;
const auto kPalm1PointColor = cv::Scalar(0, 200, 60);
const auto kPalm2PointColor = cv::Scalar(0, 60, 200);

inline cv::Point TransformToMatPoint(cv::Point2f p) {
  return {p.x * kWindowsSize, p.y * kWindowsSize};
}
}  // namespace

PalmDrawer::PalmDrawer()
    : img{kWindowsSize, kWindowsSize, CV_8UC3, cv::Scalar::all(0)} {
  cv::namedWindow(kPalmWindowName);
  std::cout << "PalmDrawer created\r\n";
}
void PalmDrawer::SetPalmInfo(mediapipe::Timestamp t, Palm _palm1,
                             Palm _palm2) {
  std::lock_guard<std::mutex> lock(mutex);
  if (lastTimestamp > t) return;

  lastTimestamp = t;
  palm1 = std::move(_palm1);
  palm2 = std::move(_palm2);
}
void PalmDrawer::Show() {  // call only from main thread
  std::lock_guard<std::mutex> lock(mutex);
  UpdateImage();
  cv::imshow(kPalmWindowName, img);
}
PalmDrawer& PalmDrawer::GetInst() {
  static PalmDrawer inst;
  return inst;
}
void PalmDrawer::UpdateImage() {
  img.setTo(kWindowsBackground);
  DrawPalm(palm1, kPalm1PointColor);
  DrawPalm(palm2, kPalm2PointColor);
}
void PalmDrawer::DrawPalm(const Palm& palm, cv::Scalar color) {
  for (size_t i = 0; i < palm.GetPointsCount(); ++i) {
    cv::Point p = TransformToMatPoint(palm.GetPoint(i));
    cv::circle(img, p, kPalmPointRadius, color);
    p.x += kPalmPointRadius + 2;
    cv::addText(img, std::to_string(i), p, "Hack", -1, color);
  }
}