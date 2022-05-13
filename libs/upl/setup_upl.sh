#!/bin/bash

THIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
cd $THIS_DIR

wget -c --read-timeout=5 --tries=10 https://gmap.pucrs.br/public_data/spbench/libs/upl/upl.tar.xz

tar -xf upl.tar.xz
rm upl.tar.xz
