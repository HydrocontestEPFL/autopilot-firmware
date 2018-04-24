cc_library(
    name = "sbus",
    srcs = ["sbus.c"],
    hdrs = ["sbus.h"],
    visibility = ["//visibility:public"],
    deps = ["//:messages"],
)

cc_test(
    name = "sbus-test",
    size = "small",
    srcs = ["sbus_decode_test.cpp"],
    deps = [
        ":sbus",
        "@ch_cvra_test_runner//:main",
    ],
)
