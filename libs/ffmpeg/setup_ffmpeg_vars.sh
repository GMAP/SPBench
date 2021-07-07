#!/bin/bash

LIB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"

FFMPEG_HOME=$LIB_DIR/ffmpeg-3.4.8/build
export PATH=${FFMPEG_HOME}/bin:$PATH
export LD_LIBRARY_PATH=${FFMPEG_HOME}/lib:${FFMPEG_HOME}/include:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=${FFMPEG_HOME}/lib/pkgconfig:$PKG_CONFIG_PATH
