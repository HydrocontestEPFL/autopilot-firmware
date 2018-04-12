cc_library(
    name = "fatfs",
    srcs = glob(["ff.c", "*.h"]),
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    include_prefix = "fatfs",
)

cc_library(
    name = "fatfs-test-support",
    srcs = ["fatfs_simulated_diskio.cpp"],
    deps = [":fatfs"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "fatfs-test",
    srcs = ["fatfs_test.cpp"],
    deps = [":fatfs", ":fatfs-test-support", "@ch_cvra_test_runner//:main"],
    size = "small",
)

