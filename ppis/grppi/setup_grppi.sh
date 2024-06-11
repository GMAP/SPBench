#!/bin/sh

# Function to determine the directory of the currently executed or sourced script
get_script_dir() {
    if [ -n "$BASH_SOURCE" ]; then
        # Bash
        script="$BASH_SOURCE"
    elif [ -n "$ZSH_VERSION" ]; then
        # Zsh
        script="${(%):-%N}"
    else
        # Fallback for other shells
        script="$0"
    fi

    # Resolve the absolute path to the script
    script_dir=$(cd "$(dirname "$script")" && pwd)
    echo "$script_dir"
}

THIS_DIR=$(get_script_dir)
cd $THIS_DIR

FILE=grppi_install_pkg.tar.gz

if [ ! -f "$FILE" ]; then
    echo "Downloading $FILE..."
    wget -c --read-timeout=5 --tries=10 https://gmap.pucrs.br/public_data/spbench/libs/grppi/$FILE
fi

tar -xzvf $FILE

## FF export
export PATH=$PATH:$THIS_DIR/fastflow-2.2.0/

## TBB export
export PATH=$PATH:$THIS_DIR/../tbb/tbb/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$THIS_DIR/../tbb/tbb/
export LD_LIBRARY_PATH=${TBB_HOME}:${TBB_HOME}/include:$LD_LIBRARY_PATH

## GrPPI Install 
cd grppi-0.4.0
mkdir build
cd build
cmake .. -DGRPPI_UNIT_TEST_ENABLE=OFF -DGRPPI_EXAMPLE_APPLICATIONS_ENABLE=OFF -DGRPPI_TBB_ENABLE=ON -DGRPPI_FF_ENABLE=ON -DCMAKE_INSTALL_PREFIX="${THIS_DIR}/grppi-0.4.0/grppi/"
make install -j$(nproc)
