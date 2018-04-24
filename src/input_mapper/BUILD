cc_library(
    name = "input_mapper",
    srcs = ["input_mapper.c"],
    hdrs = ["input_mapper.h"],
    visibility = ["//visibility:public"],
    deps = ["//:messages", "@ch_cvra_parameter//:parameter",],
)

cc_test(
    name = "input_mapper-test",
    size = "small",
    srcs = ["test_input_mapper.cpp"],
    deps = [
        ":input_mapper",
        "@ch_cvra_parameter//:parameter_port_mock",
        "@ch_cvra_test_runner//:main",
    ],
)
