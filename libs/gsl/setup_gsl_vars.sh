#!/bin/bash

LIB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"

GSL_HOME=$LIB_DIR/gsl-2.6/build
export PATH=${GSL_HOME}/bin:$PATH
export LD_LIBRARY_PATH=${GSL_HOME}/lib:${GSL_HOME}/include:$LD_LIBRARY_PATH
export LD_RUN_PATH=${GSL_HOME}/lib:$LD_RUN_PATH
export PKG_CONFIG_PATH=${GSL_HOME}/lib/pkgconfig:$PKG_CONFIG_PATH
