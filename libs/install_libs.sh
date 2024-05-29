#!/bin/sh

DEPENDENCIES=

if [ "$1" = "lane_detection" ] || [ "$1" = "person_recognition" ]; then
    DEPENDENCIES="yasm ffmpeg opencv upl"
elif [ "$1" = "ferret" ]; then
    DEPENDENCIES="gsl jpeg upl"
elif [ "$1" = "bzip2" ]; then
    DEPENDENCIES="bzlib upl"
else
    DEPENDENCIES="bzlib yasm ffmpeg opencv gsl jpeg upl"
fi

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
            sleep 1
            if ! prompt_users; then
                cd "$THIS_DIR"
                return 1
            fi
            ;;
    esac
    return 0
}    

# Iterate the string variable using for loop
for val in $DEPENDENCIES; do

    THIS_DIR=$(cd "$(dirname "$0")" && pwd)
    echo "---------------------------------------"
    echo " Installing $val..."
    echo "---------------------------------------"
    if ! . "$THIS_DIR/$val/setup_$val.sh"; then
        echo "*************** ERROR *****************"
        echo ""
        echo " SPBench failed to install $val. Please check the error messages above."
        echo ""
        echo " The $val library can be found at $THIS_DIR/$val. You can try to install it manually."
        echo ""
        echo "***************************************"
        if ! prompt_users; then
            cd "$THIS_DIR"
            return 1
        fi
    fi
    . "$THIS_DIR/setup_${val}_vars.sh"

    cd ..

    echo "DONE!"
done

