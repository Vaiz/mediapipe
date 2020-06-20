#pragma once
#include "mediapipe/framework/timestamp.h"
#include "palm.h"

class StabilizedPalm {
  static constexpr size_t kArraySize = 100;
  static constexpr size_t kArrayTimeLengthMSec = 200;
  static constexpr size_t kArrayTimeLengthMicroseconds =
      kArrayTimeLengthMSec * 1000;
  static constexpr size_t kMSecPerUnit = kArrayTimeLengthMSec / kArraySize;

  struct PalmVersion {
    Palm palm;
    mediapipe::Timestamp timestamp{0};
  };

 public:
  void Update(mediapipe::Timestamp timestamp, Palm palm) {
    if (lastTimestamp < timestamp) lastTimestamp = timestamp;

    const auto pos = GetPosition(timestamp);
    auto& curPalm = palmCollection[pos];
    if (curPalm.timestamp < timestamp) {
      curPalm.timestamp = timestamp;
      curPalm.palm = palm;
    }
  }
  Palm GetPalm() const {
    Palm stabilizedPalm;
    for (const auto& ver : palmCollection) {
      if (!IsTooOld(ver.timestamp)) stabilizedPalm.Merge(ver.palm);
    }
    return stabilizedPalm;
  }
  Finger GetFinger(Fingers fingerType) {
    Finger stabilizedFinger;
    for (const auto& ver : palmCollection) {
      if (!IsTooOld(ver.timestamp)) stabilizedFinger.Merge(ver.palm.GetFinger(fingerType));
    }
    return stabilizedFinger;
  }
 private:
  bool IsTooOld(mediapipe::Timestamp t) const {
    const auto diff = lastTimestamp - t;
    return diff.Microseconds() > kArrayTimeLengthMicroseconds;
  }
  static size_t GetPosition(mediapipe::Timestamp t) {
    auto msec = t.Microseconds() / 1000;
    return (msec % kArrayTimeLengthMSec) / kMSecPerUnit;
  }

 private:
  mediapipe::Timestamp lastTimestamp{0};
  std::array<PalmVersion, kArraySize> palmCollection;
};