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
#include "palm_drawer.h"

namespace mediapipe {

namespace {
constexpr char kLandmarksTag[] = "LANDMARKS";
constexpr char kPalmWindowName[] = "Palm";
constexpr int kWindowsSize = 500;
const auto kWindowsBackground = cv::Scalar(60, 60, 60);
constexpr int kPalmPointRadius = 5;
const auto kPalm1PointColor = cv::Scalar(0, 200, 60);
const auto kPalm2PointColor = cv::Scalar(0, 60, 200);
using NormalizedPointsVec = std::vector<std::pair<float, float>>;
}  // namespace
/*
class PalmDrawer {
 public:
  PalmDrawer() {
    cv::namedWindow(kPalmWindowName);
    thread = std::thread([this] { ThreadFn(); });
  }
  ~PalmDrawer() {
    {
      std::lock_guard<std::mutex> lock(mutex);
      stopFlag = true;
    }
    if (thread.joinable()) thread.join();
  }
  void SetPalmInfo(mediapipe::Timestamp t, NormalizedPointsVec palm1,
                   NormalizedPointsVec palm2) {
    std::lock_guard<std::mutex> lock(mutex);
    if (lastTimestamp > t) return;
    lastTimestamp = t;
    m_palm1 = std::move(palm1);
    m_palm2 = std::move(palm2);
  }

 private:
  void ThreadFn() noexcept {
    try {
      while (!stopFlag) {
        UpdateImage();
        cv::imshow(kPalmWindowName, img);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        //cv::waitKey(10);
      }

    } catch (const std::exception& e) {
      std::cerr << "PalmDrawer::ThreadFn: " << e.what() << "\r\n";
    }
  }
  void UpdateImage() {
    std::lock_guard<std::mutex> lock(mutex);
    img.setTo(cv::Scalar(60, 60, 60));

    for (size_t i = 0; i < m_palm1.size(); ++i) {
      cv::circle(img, MakePoint(m_palm1[i]), kPalmPointRadius,
                 kPalm1PointColor);
    }
    for (size_t i = 0; i < m_palm2.size(); ++i) {
      cv::circle(img, MakePoint(m_palm2[i]), kPalmPointRadius,
                 kPalm2PointColor);
    }
  }
  static cv::Point MakePoint(std::pair<float, float> p) {
    return {p.first * kWindowsSize, p.second * kWindowsSize};
  }

 private:
  std::mutex mutex;
  bool stopFlag{false};

  mediapipe::Timestamp lastTimestamp;
  cv::Mat img{kWindowsSize, kWindowsSize, CV_8UC3, cv::Scalar::all(0)};
  NormalizedPointsVec m_palm1;
  NormalizedPointsVec m_palm2;

  std::thread thread;
};
*/
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
    cc->GetCounter("PassThrough")->Increment();
    if (cc->Inputs().NumEntries() == 0) {
      return tool::StatusStop();
    }
    for (CollectionItemId id = cc->Inputs().BeginId();
         id < cc->Inputs().EndId(); ++id) {
      if (!cc->Inputs().Get(id).IsEmpty()) {
        std::cout << "Process " << cc->Inputs().Get(id).Name() << ": "
                  << cc->InputTimestamp().DebugString() << "\r\n";
      }
    }

    if (cc->Inputs().HasTag(kLandmarksTag)) {
      std::vector<NormalizedPointsVec> palms(2);
      const auto& landmarks = cc->Inputs()
                                  .Tag(kLandmarksTag)
                                  .Get<std::vector<NormalizedLandmarkList>>();
      for (size_t palmIndex = 0; palmIndex < landmarks.size() && palmIndex < 2;
           ++palmIndex) {
        const auto curLandmarkList = landmarks[palmIndex];
        std::cout << "row: " << palmIndex
                  << ", size: " << curLandmarkList.landmark_size() << "\r\n";
        for (int i = 0; i < curLandmarkList.landmark_size(); ++i) {
          const NormalizedLandmark& landmark = curLandmarkList.landmark(i);
          // std::cout << "\t" << i << ": (" << landmark.x() << ":" <<
          // landmark.y()
          //          << ")\r\n";
          palms.at(palmIndex).emplace_back(landmark.x(), landmark.y());
        }
      }

      PalmDrawer::GetInst().SetPalmInfo(
          cc->InputTimestamp(), std::move(palms.at(0)), std::move(palms.at(1)));
    }

    return ::mediapipe::OkStatus();
  }
};
REGISTER_CALCULATOR(MyPassThroughCalculator);

}  // namespace mediapipe
