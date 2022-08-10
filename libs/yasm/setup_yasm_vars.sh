#!/bin/bash

LIB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"

YASM_HOME=$LIB_DIR/yasm-1.3.0/build
export PATH=${YASM_HOME}/bin:$PATH
export LD_LIBRARY_PATH=${YASM_HOME}/lib:${YASM_HOME}/include:$LD_LIBRARY_PATH
