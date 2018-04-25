cc_library(
    name = "output_mixer",
    srcs = ["output_mixer.c"],
    hdrs = ["output_mixer.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//:messages",
        "@ch_cvra_parameter//:parameter",
    ],
)

cc_test(
    name = "output_mixer-test",
    size = "small",
    srcs = ["test_output_mixer.cpp"],
    deps = [
        ":output_mixer",
        "@ch_cvra_parameter//:parameter_port_mock",
        "@ch_cvra_test_runner//:main",
    ],
)
