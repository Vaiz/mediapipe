// Copyright 2019 The MediaPipe Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <opencv2/opencv.hpp>
#include <thread>

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/port/canonical_errors.h"
#include "mouse_control.h"
#include "hand_model/palm.h"
#include "palm_drawer.h"

namespace mediapipe {

namespace {
constexpr char kLandmarksTag[] = "LANDMARKS";
using NormalizedPointsVec = std::vector<cv::Point2f>;
}  // namespace

class MyPassThroughCalculator final : public CalculatorBase {
 public:
  static ::mediapipe::Status GetContract(CalculatorContract* cc) {
    for (CollectionItemId id = cc->Inputs().BeginId();
         id < cc->Inputs().EndId(); ++id) {
      cc->Inputs().Get(id).SetAny();
    }
    for (CollectionItemId id = cc->InputSidePackets().BeginId();
         id < cc->InputSidePackets().EndId(); ++id) {
      cc->InputSidePackets().Get(id).SetAny();
    }
    if (cc->OutputSidePackets().NumEntries() != 0) {
      if (!cc->InputSidePackets().TagMap()->SameAs(
              *cc->OutputSidePackets().TagMap())) {
        return ::mediapipe::InvalidArgumentError(
            "Input and output side packets to MyPassThroughCalculator must use "
            "matching tags and indexes.");
      }
      for (CollectionItemId id = cc->InputSidePackets().BeginId();
           id < cc->InputSidePackets().EndId(); ++id) {
        cc->OutputSidePackets().Get(id).SetSameAs(
            &cc->InputSidePackets().Get(id));
      }
    }
    return ::mediapipe::OkStatus();
  }

  ::mediapipe::Status Open(CalculatorContext* cc) final {
    cc->SetOffset(TimestampDiff(0));
    return ::mediapipe::OkStatus();
  }

  ::mediapipe::Status Process(CalculatorContext* cc) final {
    if (cc->Inputs().NumEntries() == 0) {
      return tool::StatusStop();
    }

    if (cc->Inputs().HasTag(kLandmarksTag)) {
      std::vector<Palm> palms(2);
      const auto& landmarks = cc->Inputs()
                                  .Tag(kLandmarksTag)
                                  .Get<std::vector<NormalizedLandmarkList>>();
      for (size_t palmIndex = 0; palmIndex < landmarks.size() && palmIndex < 2;
           ++palmIndex) {
        const auto curLandmarkList = landmarks[palmIndex];
        assert(curLandmarkList.landmark_size() == Palm::GetPointsCount());
        for (int i = 0; i < curLandmarkList.landmark_size(); ++i) {
          const NormalizedLandmark& landmark = curLandmarkList.landmark(i);
          palms.at(palmIndex).SetPoint(i, landmark.x(), landmark.y());
        }
      }

      if (palms.at(0).IsValid()) {
        cv::Point2f indexTip = palms.at(0).GetFinger(Fingers::Index).GetTip();
        MouseControl::SetCursorRelativePos(indexTip.x, indexTip.y);
      }

      PalmDrawer::GetInst().SetPalmInfo(
          cc->InputTimestamp(), std::move(palms.at(0)), std::move(palms.at(1)));
    }

    return ::mediapipe::OkStatus();
  }
};
REGISTER_CALCULATOR(MyPassThroughCalculator);

}  // namespace mediapipe
