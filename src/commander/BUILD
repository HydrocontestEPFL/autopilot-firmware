cc_library(
    name = "commander",
    srcs = ["commander.c"],
    hdrs = ["commander.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//:messages",
        "//lib/error",
    ],
)

cc_test(
    name = "test_commander",
    size = "small",
    srcs = ["test_commander.cpp"],
    deps = [
        ":commander",
        "@ch_cvra_test_runner//:main",
    ],
)
