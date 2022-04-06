#!/bin/bash

bazel clean
bazel build ...

bazel test //test:test

rm -rf build
mkdir build

cp bazel-bin/app/ipc_send build
cp bazel-bin/app/ipc_receive build
cp bazel-testlogs/test/test/test.log build
