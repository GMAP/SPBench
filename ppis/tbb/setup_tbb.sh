#!/bin/bash

FILE=tbb.tar.gz

if [ ! -f "$FILE" ]; then
    echo "Downloading $FILE..."
    wget -c --read-timeout=5 --tries=10 https://gmap.pucrs.br/public_data/spbench/libs/tbb/$FILE
fi


tar -xf $FILE
cd tbb
cmake CMakeLists.txt
make tbb -j$(nproc)
cd ..
