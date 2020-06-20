#build
bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 --cxxopt="-std=c++17" mediapipe/examples/desktop/hand_gesture_tracking:hand_gesture_tracking
#run default graph
bazel-bin/mediapipe/examples/desktop/hand_gesture_tracking/hand_gesture_tracking --calculator_graph_config_file=mediapipe/graphs/hand_tracking/multi_hand_tracking_desktop_live.pbtxt
#run test graph
bazel-bin/mediapipe/examples/desktop/hand_gesture_tracking/hand_gesture_tracking --calculator_graph_config_file=mediapipe/examples/desktop/hand_gesture_tracking/multi_hand_gesture_tracking_desktop_live.pbtxt

