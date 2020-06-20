#load("//mediapipe/framework/tool:mediapipe_graph.bzl", "mediapipe_simple_subgraph")

cc_binary(
    name = "hand_gesture_tracking",
    srcs = glob(["*.cc","*.h"]),
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/formats:image_frame",
        "//mediapipe/framework/formats:image_frame_opencv",
        "//mediapipe/framework/port:commandlineflags",
        "//mediapipe/framework/port:file_helpers",
        "//mediapipe/framework/port:opencv_highgui",
        "//mediapipe/framework/port:opencv_imgproc",
        "//mediapipe/framework/port:opencv_video",
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_calculator_helper",
        "//mediapipe/gpu:gpu_buffer",
        "//mediapipe/gpu:gpu_shared_data_internal",
        "//mediapipe/graphs/hand_tracking:multi_hand_mobile_calculators", # only mobile calculators works with gpu
        "@system_libs//:x11",
    ],
)
