#!/bin/bash

bazel clean
bazel build //app:ipc_send
bazel build //app:ipc_receive

rm -rf build
mkdir build

cp bazel-bin/app/ipc_send build
cp bazel-bin/app/ipc_receive build

cp bazel-bin/app/ipc_send data
cp bazel-bin/app/ipc_receive data

bazel coverage test:test --combined_report=lcov --instrumentation_filter="//lib[/:]"

cp bazel-testlogs/test/test/test.log build
cp bazel-out/_coverage/_coverage_report.dat build

genhtml -o html build/_coverage_report.dat
