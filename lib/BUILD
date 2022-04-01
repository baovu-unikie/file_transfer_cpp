cc_library(
    name = "IPC",
    hdrs = ["IPC.h"],
    visibility = ["//visibility:public"],
    deps = ["//lib:ChronoTime"],
)

cc_library(
    name = "IPCHandler",
    srcs = ["IPCHandler.cpp"],
    hdrs = ["IPCHandler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//lib:IPC",
        "//lib:IPCMsgQReceive",
        "//lib:IPCMsgQSend",
    ],
)

cc_library(
    name = "ChronoTime",
    srcs = ["ChronoTime.cpp"],
    hdrs = ["ChronoTime.h"],
    visibility = ["//visibility:public"],
    deps = [],
)

cc_library(
    name = "FileHandler",
    srcs = ["FileHandler.cpp"],
    hdrs = ["FileHandler.h"],
    linkopts = ["-lboost_filesystem"],
    visibility = ["//visibility:public"],
    deps = [],
)

cc_library(
    name = "IPCMsgQSend",
    srcs = ["IPCMsgQSend.cpp"],
    hdrs = ["IPCMsgQSend.h"],
    linkopts = ["-lrt"],
    visibility = ["//visibility:public"],
    deps = [
        "//lib:FileHandler",
        "//lib:IPC",
    ],
)

cc_library(
    name = "IPCMsgQReceive",
    srcs = ["IPCMsgQReceive.cpp"],
    hdrs = ["IPCMsgQReceive.h"],
    linkopts = ["-lrt"],
    visibility = ["//visibility:public"],
    deps = [
        "//lib:FileHandler",
        "//lib:IPC",
    ],
)
