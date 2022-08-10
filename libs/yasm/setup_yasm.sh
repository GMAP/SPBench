#!/bin/bash

THIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
cd $THIS_DIR

FILE="yasm-1.3.0.tar.gz"

if [ ! -f "$FILE" ]; then
    echo "Downloading $FILE..."
    wget -c --read-timeout=5 --tries=10 https://gmap.pucrs.br/public_data/spbench/libs/yasm/$FILE
fi

LIB_DIR="yasm-1.3.0"

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
PREFIX="$PWD/build"
PATH="$PREFIX/bin:$PATH" 
./configure --prefix=$PREFIX
make -j$(nproc)
make install
cd ..
