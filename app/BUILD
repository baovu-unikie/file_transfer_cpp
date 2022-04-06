cc_binary(
    name = "ipc_receive",
    srcs = ["ipc_receive.cpp"],
    data = [
        "//data",
    ],
    linkopts = ["-lboost_filesystem"],
    deps = ["//lib:IPC"],
)

cc_binary(
    name = "ipc_send",
    srcs = ["ipc_send.cpp"],
    data = [
        "//data",
    ],
    linkopts = ["-lboost_filesystem"],
    deps = ["//lib:IPC"],
)
