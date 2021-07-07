#!/bin/bash

THIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
cd $THIS_DIR

wget https://gmap.pucrs.br/spbench/libs/upl/upl.tar.xz

tar -xf upl.tar.xz
rm upl.tar.xz
