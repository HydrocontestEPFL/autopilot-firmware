# vim: set ft=bzl:
cc_library(
    name = "CppUTestCore",
    srcs = glob(["cpputest-3.8/src/CppUTest/*.cpp"]),
    hdrs = glob(["cpputest-3.8/include/CppUTest/*.h"]),
    strip_include_prefix="cpputest-3.8/include/",
)

cc_library(
    name = "CppUTestExt",
    srcs = glob(["cpputest-3.8/src/CppUTestExt/*.cpp"]),
    hdrs = glob(["cpputest-3.8/include/CppUTestExt/*.h"]),
    strip_include_prefix="cpputest-3.8/include/",
)

cc_library(
    name = "GccPlatform",
    srcs = glob(["cpputest-3.8/src/Platforms/Gcc/*.cpp"])
)

cc_library(
    name = "CppUTest",
    deps = ["CppUTestCore", "CppUTestExt", "GccPlatform"],
    visibility = ["//visibility:public"],
)
