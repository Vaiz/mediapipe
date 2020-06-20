#pragma once

#include <array>
#include <opencv2/core/types.hpp>

inline float PointsDistance(cv::Point2f p1, cv::Point2f p2) {
  return cv::norm(p1 - p2);
}

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

enum class Fingers {
  Thumb = 0,
  Index = 1,
  Middle = 2,
  Ring = 3,
  Pinky = 4,
};

using PalmPointsArray = std::array<cv::Point2f, 21>;

class Palm {
  enum Points {
    WristPoint = 0,
    ThumbRoot = 1,
    ThumbKnucle1 = 2,
    ThumbKnucle2 = 3,
    ThumbTip = 4,
    IndexRoot = 5,
    IndexKnucle1 = 6,
    IndexKnucle2 = 7,
    IndexTip = 8,
    MiddleRoot = 9,
    MiddleKnucle1 = 10,
    MiddleKnucle2 = 11,
    MiddleTip = 12,
    RingRoot = 13,
    RingKnucle1 = 14,
    RingKnucle2 = 15,
    RingTip = 16,
    PinkyRoot = 17,
    PinkyKnucle1 = 18,
    PinkyKnucle2 = 19,
    PinkyTip = 20,
  };

 public:
  Palm() = default;
  Palm(PalmPointsArray points) : points(points) {}

  void SetPoint(size_t index, float x, float y) {
    points.at(index).x = x;
    points.at(index).y = y;
  }
  cv::Point2f GetPoint(size_t index) const { return points.at(index); }
  static constexpr size_t GetPointsCount() { return 21; }
  Finger GetFinger(Fingers fingerType) {
    const size_t start = 1 + static_cast<size_t>(fingerType) * 4;
    const auto startIter = points.cbegin() + start;

    FingerPointsArray arr;
    std::copy_n(startIter, 4, arr.begin());
    return Finger(points[0], arr);
  }
  bool IsValid() const {
    constexpr auto p = 0.01f;
    cv::Point2f wrist = points[WristPoint];
    return p < wrist.x && p < wrist.y;
  }

 private:
  PalmPointsArray points;
};
