#!/bin/sh

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
BUILD_DIR="${SCRIPT_DIR}/build"

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}
test -e CMakeCache.txt || cmake ..
cmake --build .
