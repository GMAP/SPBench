#!/bin/bash

LIB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"

OPENCV_HOME=$LIB_DIR/opencv-2.4.13.6
export PATH=${OPENCV_HOME}/bin:$PATH
export LD_LIBRARY_PATH=${OPENCV_HOME}/lib:${OPENCV_HOME}/include:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=${OPENCV_HOME}/lib/pkgconfig:$PKG_CONFIG_PATH
