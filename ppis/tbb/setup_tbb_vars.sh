#!/bin/bash

LIB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"

TBB_HOME=$LIB_DIR/tbb
export LD_LIBRARY_PATH=${TBB_HOME}:${TBB_HOME}/include:$LD_LIBRARY_PATH
