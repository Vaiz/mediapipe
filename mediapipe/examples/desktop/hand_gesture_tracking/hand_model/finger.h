#pragma once
#include <array>
#include <opencv2/core/types.hpp>


class Finger {
  enum Points {
    Root = 0,
    Knucle1 = 1,
    Knucle2 = 2,
    Tip = 3,
  };

 public:
  static constexpr size_t kFingerPointsCount = 4;
  using FingerPointsArray = std::array<cv::Point2f, kFingerPointsCount>;


 public:
  Finger() = default;
  Finger(cv::Point2f wrist, FingerPointsArray points)
      : wrist(wrist), points(points) {}
  cv::Point2f GetTip() const { return points[Tip]; }

  void Add(const Finger& other) {
    if (!IsValid()) {
      wrist = other.wrist;
      points = other.points;
      return;
    }
    wrist = (wrist + other.wrist) / 2;
    for (size_t i = 0; i < kFingerPointsCount; ++i)
      points[i] = (points[i] + other.points[i]);
  }
  void Merge(const Finger& other) {
    if (!IsValid()) {
      wrist = other.wrist;
      points = other.points;
      return;
    }
    wrist = (wrist + other.wrist) / 2;
    for (size_t i = 0; i < kFingerPointsCount; ++i)
      points[i] = (points[i] + other.points[i]) / 2;
  }
  bool IsValid() const {
    constexpr auto p = 0.01f;
    return p < wrist.x && p < wrist.y;
  }
  void Divide(float value) {
    for (size_t i = 0; i < kFingerPointsCount; ++i)
      points[i] /= value;
  }

 private:
  cv::Point2f wrist;
  FingerPointsArray points;
};