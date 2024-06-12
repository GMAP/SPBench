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

DEPENDENCIES=

# Declare dependencies based on input argument
case "$1" in
    lane_detection|person_recognition)
        DEPENDENCIES=("yasm" "ffmpeg" "opencv" "upl")
        ;;
    ferret)
        DEPENDENCIES=("yasm" "gsl" "jpeg" "upl")
        ;;
    bzip2)
        DEPENDENCIES=("bzlib" "upl")
        ;;
    *)
        DEPENDENCIES=("bzlib" "yasm" "ffmpeg" "opencv" "gsl" "jpeg" "upl")
        ;;
esac

# Function to prompt the user for input
prompt_users() {
    echo "Please select an option:"
    echo " 1) Proceed with the installation of the next libraries (WARNING: This may cause errors if the libraries depend on this one!)."
    echo " 2) Stop the libraries installation."
    echo "Enter your choice: "
    read choice
    case $choice in
        1)
            echo "Trying to install the remaining libraries..."
            ;;
        2)
            echo "Exiting the installation..."
            exit 1
            ;;
        *)
            echo ""
            echo " ERROR: INVALID CHOICE!"
            echo ""
            return 1
            ;;
    esac
    return 0
}    

# Store the original directory
original_dir=$(get_script_dir)

# Iterate over the dependencies and attempt to install each one
for val in "${DEPENDENCIES[@]}"; do
    
    cd "$original_dir/$val"
    
    echo "---------------------------------------"
    echo " Installing $val..."
    echo "---------------------------------------"
    if ! . "$original_dir/$val/setup_$val.sh"; then
        echo "*************** ERROR *****************"
        echo ""
        echo " SPBench failed to install $val. Please check the error messages above."
        echo ""
        echo " The $val library can be found at $original_dir/$val. You can try to install it manually."
        echo ""
        echo "***************************************"
        prompt_users
        if [ $? -ne 0 ]; then
            cd "$original_dir"
            return 1
        fi
    fi
    . $original_dir/$val/setup_${val}_vars.sh
    cd "$original_dir"
    echo "DONE!"
done