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

cd $(get_script_dir)

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
