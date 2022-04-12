cc_test(
    name = "test",
    srcs = [
        "test.cpp",
    ],
    data = ["//data"],
    linkopts = ["-lgtest", "-lpthread"],
    deps = [
        "//lib:ChronoTime",
        "//lib:FileHandler",
        "//lib:IPCHandler",
        "//test:ChronoTimeTestSuite",
        "//test:DummyFileGenerator",
        "//test:IPCHandlerTestSuite",
    ],
)

cc_library(
    name = "IPCHandlerTestSuite",
    hdrs = ["IPCHandlerTestSuite.h"],
    linkopts = ["-lgtest"],
    visibility = ["//visibility:public"],
    deps = ["//lib:IPCHandler"],
)

cc_library(
    name = "ChronoTimeTestSuite",
    hdrs = ["ChronoTimeTestSuite.h"],
    linkopts = ["-lgtest"],
    visibility = ["//visibility:public"],
    deps = ["//lib:ChronoTime"],
)

cc_library(
    name = "DummyFileGenerator",
    srcs = ["dummy_file_generator.cpp"],
    hdrs = ["dummy_file_generator.h"],
    linkopts = ["-lboost_filesystem"],
)
