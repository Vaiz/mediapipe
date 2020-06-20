#pragma once
#include <array>
#include <opencv2/core/types.hpp>

using FingerPointsArray = std::array<cv::Point2f, 4>;

class Finger {
  enum Points {
    Root = 0,
    Knucle1 = 1,
    Knucle2 = 2,
    Tip = 3,
  };

 public:
  Finger() = default;
  Finger(cv::Point2f wrist, FingerPointsArray points)
      : wrist(wrist), points(points) {}
  cv::Point2f GetTip() const { return points[Tip]; }

  void Merge(const Finger& other) {
    if (!IsValid()) {
      wrist = other.wrist;
      points = other.points;
      return;
    }

    wrist = (wrist + other.wrist) / 2;
    for (size_t i = 0; i < 4; ++i)
      points[i] = (points[i] + other.points[i]) / 2;
  }
  bool IsValid() const {
    constexpr auto p = 0.01f;
    return p < wrist.x && p < wrist.y;
  }

 private:
  cv::Point2f wrist;
  FingerPointsArray points;
};