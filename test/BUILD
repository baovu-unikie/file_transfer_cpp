cc_test(
    name = "test",
    srcs = [
        "test.cpp",
    ],
    linkopts = [
        "-lgtest_main",
        "-lgtest",
        "-lpthread",
        "-lboost_filesystem",
    ],
    deps = [
        "//lib:IPC",
    ],
)
