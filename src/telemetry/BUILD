cc_library(
    name = "rpc",
    srcs = ["rpc.c"],
    hdrs = ["rpc.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//:messages",
        "@com_github_nanopb_nanopb//:nanopb",
    ],
)

cc_test(
    name = "rpc_test",
    size = "small",
    srcs = ["tests/rpc.cpp"],
    deps = [
        ":rpc",
        "@ch_cvra_test_runner//:main",
        "//:messages",
    ],
)
