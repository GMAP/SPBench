#!/bin/bash

LIB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"

export PATH=$LIB_DIR/bin:$PATH
export LD_LIBRARY_PATH=$LIB_DIR/lib:$PWD/include:$LD_LIBRARY_PATH
