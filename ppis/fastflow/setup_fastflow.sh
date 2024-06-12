#!/usr/bin/env bash

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

wget -c --read-timeout=5 --tries=10 https://gmap.pucrs.br/public_data/spbench/libs/fastflow/fastflow.tar.gz

tar -xf fastflow.tar.gz

rm fastflow.tar.gz
