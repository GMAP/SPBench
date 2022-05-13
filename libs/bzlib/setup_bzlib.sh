#!/bin/bash
THIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
cd $THIS_DIR

FILE=bzip2-1.0.8.tar.gz

LIB_DIR=bzip2-1.0.8

if [ ! -f "$FILE" ]; then
	echo "Downloading $FILE..."
	wget -c --read-timeout=5 --tries=10 https://gmap.pucrs.br/public_data/spbench/libs/bzip2/$FILE
fi

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
make -j$(nproc)
make install PREFIX="$PWD/.."
cd ..
rm -r $LIB_DIR
