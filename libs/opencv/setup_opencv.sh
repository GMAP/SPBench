#!/bin/bash

THIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
cd $THIS_DIR

FILE=opencv-2.4.13.6.tar.gz

if [ ! -f "$FILE" ]; then
    echo "Downloading $FILE..."
    wget -c --read-timeout=5 --tries=10 https://gmap.pucrs.br/public_data/spbench/libs/opencv/$FILE
fi

LIB_DIR=opencv-2.4.13.6

if [ -d "$LIB_DIR" ]; then
	echo "Previous installation found."
	echo "Do you want to delete and reinstall it? [yes/no]"
	read ANSWER
	re2='[Yy]es|YES|[Yy]'
	if ! [[ $ANSWER =~ $re2 ]] ; then
		echo "Installation cancelled!"
		exit 1
	fi
	rm -rf $LIB_DIR
fi

tar -xf $FILE
cd $LIB_DIR
mkdir build
cd build
cmake -DCMAKE_CXX_FLAGS="--std=c++11" -DBUILD_PNG=ON -DBUILD_EXAMPLES=OFF -DWITH_FFMPEG=ON -DOPENCV_FFMPEG_SKIP_BUILD_CHECK=ON -DWITH_CUDA=OFF -DCMAKE_INSTALL_PREFIX=../ ..
make -j$(nproc)
make install
cd ..
