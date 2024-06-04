#!/bin/bash

THIS_DIR=$(cd "$(dirname "$0")" && pwd)
cd $THIS_DIR

if [ ! -f "upl.tar.xz" ]; then
    wget -c --read-timeout=5 --tries=10 https://gmap.pucrs.br/public_data/spbench/libs/upl/upl.tar.xz
fi

if ! tar -xf upl.tar.xz; then
    echo "Failed to extract upl.tar.xz"
    echo "Trying to download it again..."
    rm -rf upl.tar.xz
    wget -c --read-timeout=5 --tries=10 https://gmap.pucrs.br/public_data/spbench/libs/upl/upl.tar.xz
fi

if ! tar -xf upl.tar.xz; then
    echo "Failed to extract upl.tar.xz"
    return 1
fi

return 0