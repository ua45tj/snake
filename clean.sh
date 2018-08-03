#!/bin/sh

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
BUILD_DIR="${SCRIPT_DIR}/build"

echo "Removing ${BUILD_DIR}"
rm -rf ${BUILD_DIR}
