cc_library(
    name = "madgwick",
    srcs = ["madgwick.c"],
    hdrs = ["madgwick.h"],
)

cc_test(
    name = "madgwick-test",
    srcs = ["test_madgwick.cpp"],
    deps = [":madgwick", "@ch_cvra_test_runner//:main"],
    size = "small",
)
