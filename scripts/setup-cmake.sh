#!/usr/bin/env bash

######################################################################
# @author      : kyle (kyleimap.cc)
# @file        : setup-cmake
# @created     : Wednesday Oct 18, 2023 04:24:21 PDT
#
# @description : Prepare a CMake build environment, build, and test.
######################################################################

set -eux

mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. && make all test && ctest
