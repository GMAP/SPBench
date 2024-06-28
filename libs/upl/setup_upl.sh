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

THIS_DIR=""
THIS_SCRIPT="setup_upl.sh"

# Check if $1 is set
if [ -z "$1" ]; then
	echo "The directory of the currently executed script was not passed as an argument."
	echo "Trying to determine the directory of the currently executed script..."
	THIS_DIR=$(get_script_dir)
else
	THIS_DIR=$1
fi

# Check if the script exists on THIS_DIR
if [ ! -f "$THIS_DIR/$THIS_SCRIPT" ]; then
	echo "The script $THIS_DIR/$THIS_SCRIPT was not found."
	echo "Please make sure you are running this script from the same directory where the script is, or pass this script path as an argument."
	return 1
fi

cd $THIS_DIR
	echo "Entering $THIS_DIR ..."

if [ ! -f "upl.tar.gz" ]; then
    wget -c --read-timeout=5 --tries=10 https://gmap.pucrs.br/public_data/spbench/libs/upl/upl.tar.gz
fi

if ! tar -xf upl.tar.gz; then
    echo "Failed to extract upl.tar.gz"
    echo "Trying to download it again..."
    rm -rf upl.tar.gz
    wget -c --read-timeout=5 --tries=10 https://gmap.pucrs.br/public_data/spbench/libs/upl/upl.tar.gz
fi

if ! tar -xf upl.tar.gz; then
    echo "Failed to extract upl.tar.gz"
    return 1
fi

return 0