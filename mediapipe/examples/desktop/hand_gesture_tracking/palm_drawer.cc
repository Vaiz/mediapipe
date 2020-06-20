#include "palm_drawer.h"

#include "mouse_control.h"

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
inline std::string TimeStampToTime(mediapipe::Timestamp t) {
  const auto kTimestampUnitsPerSecond = static_cast<std::uint64_t>(
      mediapipe::Timestamp::kTimestampUnitsPerSecond);
  const auto microseconds = t.Microseconds() % kTimestampUnitsPerSecond;
  const auto seconds = (t.Microseconds() / kTimestampUnitsPerSecond) % 60;
  const auto minutes = (t.Microseconds() / kTimestampUnitsPerSecond / 60) % 60;
  const auto hours = t.Microseconds() / kTimestampUnitsPerSecond / 60 / 60;
  std::stringstream ss;
  ss << std::setw(2) << std::setfill('0') << std::right << hours << ':'
     << std::setw(2) << std::setfill('0') << std::right << minutes << ':'
     << std::setw(2) << std::setfill('0') << std::right << seconds << '.'
     << std::setw(6) << std::setfill('0') << std::right << microseconds;
  return ss.str();
}
}  // namespace

PalmDrawer::PalmDrawer()
    : img{kWindowsSize, kWindowsSize, CV_8UC3, cv::Scalar::all(0)} {
  cv::namedWindow(kPalmWindowName);
  std::cout << "PalmDrawer created\r\n";
}
void PalmDrawer::SetPalmInfo(mediapipe::Timestamp t, Palm _palm1, Palm _palm2) {
  std::lock_guard<std::mutex> lock(mutex);

  spalm1.Update(t, _palm1);
  spalm2.Update(t, _palm2);

  {
    auto finger = spalm1.GetFinger(Fingers::Index);
    if (finger.IsValid()) {
      cv::Point2f indexTip = spalm1.GetFinger(Fingers::Index).GetTip();
      MouseControl::SetCursorRelativePos(indexTip.x, indexTip.y);
    }
  }

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
  std::string timestamp_str = TimeStampToTime(lastTimestamp);
  cv::addText(img, timestamp_str, {10, kWindowsSize - 30}, "Hack", -1,
              kPalm1PointColor);

  DrawPalm(spalm1.GetPalm(), kPalm1PointColor);
  DrawPalm(spalm2.GetPalm(), kPalm2PointColor);
  // DrawPalm(palm1, kPalm1PointColor);
  // DrawPalm(palm2, kPalm2PointColor);
}
void PalmDrawer::DrawPalm(const Palm& palm, cv::Scalar color) {
  if (!palm.IsValid()) return;

  for (size_t i = 0; i < Palm::kPalmPointsCount; ++i) {
    cv::Point p = TransformToMatPoint(palm.GetPoint(i));
    cv::circle(img, p, kPalmPointRadius, color);
    p.x += kPalmPointRadius + 2;
    cv::addText(img, std::to_string(i), p, "Hack", -1, color);
  }
}