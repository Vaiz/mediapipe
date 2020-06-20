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
  Finger(cv::Point2f wrist, FingerPointsArray points)
      : wrist(wrist), points(points) {}
  cv::Point2f GetTip() const { return points[Tip]; }

 private:
  cv::Point2f wrist;
  FingerPointsArray points;
};