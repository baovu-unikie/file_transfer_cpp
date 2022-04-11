cc_binary(
    name = "ipc_receive",
    srcs = ["ipc_receive.cpp"],
    deps = ["//lib:IPCHandler"],
)

cc_binary(
    name = "ipc_send",
    srcs = ["ipc_send.cpp"],
    data = ["//data"],
    deps = ["//lib:IPCHandler"],
)
