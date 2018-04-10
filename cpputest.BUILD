# vim: set ft=bzl:
cc_library(
    name = "CppUTestCore",
    srcs = glob(["src/CppUTest/*.cpp"]),
    hdrs = glob(["include/CppUTest/*.h"]),
    strip_include_prefix="include/",
)

cc_library(
    name = "CppUTestExt",
    srcs = glob(["src/CppUTestExt/*.cpp"]),
    hdrs = glob(["include/CppUTestExt/*.h"]),
    strip_include_prefix="include/",
    deps = [":CppUTestCore"],
)

cc_library(
    name = "GccPlatform",
    srcs = glob(["src/Platforms/Gcc/*.cpp"]),
    deps = [":CppUTestCore"],
)

cc_library(
    name = "CppUTest",
    deps = ["CppUTestCore", "CppUTestExt", "GccPlatform"],
    visibility = ["//visibility:public"],
)
