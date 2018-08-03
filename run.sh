#!/bin/sh

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
BUILD_DIR="${SCRIPT_DIR}/build"

cd ${BUILD_DIR}
src/snake
