#!/bin/bash

FILE=tbb.tar.gz

if [ ! -f "$FILE" ]; then
    echo "Downloading $FILE..."
    wget https://gmap.pucrs.br/spbench/libs/tbb/$FILE
fi


tar -xf $FILE
cd tbb
cmake CMakeLists.txt
make tbb -j$(nproc)
cd ..
