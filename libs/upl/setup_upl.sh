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