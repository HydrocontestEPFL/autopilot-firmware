# TODO: Generate that one too
cc_library(
    name = "messages",
    srcs = glob([
        "build/messages/*.pb.c",
        "build/messages/*.pb.h",
    ]),
    hdrs = glob(["build/messages/*.pb.h"]),
    strip_include_prefix = "build",
    deps = [
        "@com_github_nanopb_nanopb//:nanopb",
    ],
)

cc_library(
    name = "sbus",
    srcs = ["src/sbus.c"],
    hdrs = ["src/sbus.h"],
    deps = [
        ":messages",
    ],
    strip_include_prefix = "src",
)

cc_test(
    name = "sbus-test",
    srcs = ["tests/sbus_parser.cpp"],
    deps = [
        ":sbus",
        "@ch_cvra_test_runner//:main",
    ],
    size = "small",
)
