#!/bin/bash

LIB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"

JPEG_HOME=$LIB_DIR/libjpeg-turbo-1.5.2/build
export PATH=${JPEG_HOME}/bin:$PATH
export LD_LIBRARY_PATH=${JPEG_HOME}/lib:${JPEG_HOME}/include:$LD_LIBRARY_PATH
export LD_RUN_PATH=${JPEG_HOME}/lib:$LD_RUN_PATH
export PKG_CONFIG_PATH=${JPEG_HOME}/lib/pkgconfig:$PKG_CONFIG_PATH
