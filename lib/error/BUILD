cc_library(
    name = "error",
    srcs = ["error.c", "error.h"],
    hdrs = ["error.h"],
    visibility = ["//visibility:public"],
    include_prefix = "error",
)

cc_test(
    name = "error-test",
    srcs = ['tests/error.cpp', 'error.h'],
    deps = ['@ch_cvra_test_runner//:main', ':error'],
    size = "small",
)
