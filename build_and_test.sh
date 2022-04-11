#!/bin/bash

bazel clean
rm -rf build
rm -f data/ipc_send
rm -f data/ipc_receive

bazel build //app:ipc_send
bazel build //app:ipc_receive

mkdir build

cp bazel-bin/app/ipc_send build
cp bazel-bin/app/ipc_receive build

cp bazel-bin/app/ipc_send data
cp bazel-bin/app/ipc_receive data

bazel coverage test:test --combined_report=lcov --instrumentation_filter="//lib[/:]" --java_runtime_version=remotejdk_11

cp bazel-testlogs/test/test/test.log build
cp bazel-out/_coverage/_coverage_report.dat build

genhtml -o html build/_coverage_report.dat

echo -e "- build directory contains:
\t+ ipc_send,
\t+ ipc_receive,
\t+ test.log,
\t+ and coverage report (if all test PASSED)
- html directory contains coverage report in html format (index.html)"