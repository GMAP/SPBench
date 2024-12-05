#!/bin/bash

if [ -n "$ZSH_VERSION" ]; then
  LIB_DIR="$(cd "$(dirname "${(%):-%N}")" &> /dev/null && pwd)"
elif [ -n "$BASH_VERSION" ]; then
  LIB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
elif [ -n "$KSH_VERSION" ]; then
  LIB_DIR="$(cd "$(dirname "$(realpath "${.sh.file}")")" &> /dev/null && pwd)"
elif [ -n "$FISH_VERSION" ]; then
  LIB_DIR="$(cd $(dirname $(status -f)) &> /dev/null && pwd)"
else
  echo "Unsupported shell"
  exit 1
fi

BUILD_DIR=$LIB_DIR/libjpeg-turbo-1.5.2/build
export PATH=${BUILD_DIR}/bin:${PATH}
export LD_LIBRARY_PATH=${BUILD_DIR}/lib:${LD_LIBRARY_PATH}
export CPATH=${BUILD_DIR}/include:${CPATH}
export PKG_CONFIG_PATH=${BUILD_DIR}/lib/pkgconfig:${PKG_CONFIG_PATH}

echo "PATH=$BUILD_DIR/bin"
echo "LD_LIBRARY_PATH=$BUILD_DIR/lib"
echo "CPATH=$BUILD_DIR/include"
echo "PKG_CONFIG_PATH=$BUILD_DIR/lib/pkgconfig"