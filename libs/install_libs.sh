#!/bin/bash

THIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
cd $THIS_DIR

DEPENDENCIES=

if [[ "$1" == "lane_detection" ]] || [[ "$1" == "person_recognition" ]]; then
    DEPENDENCIES="yasm ffmpeg opencv upl"
elif [[ "$1" == "ferret" ]]; then
    DEPENDENCIES="gsl jpeg upl"
elif [[ "$1" == "bzip2" ]]; then
    DEPENDENCIES="bzlib upl"
else
    DEPENDENCIES="bzlib yasm ffmpeg opencv gsl jpeg upl"
fi

# Iterate the string variable using for loop
for val in $DEPENDENCIES; do
    echo "---------------------------------------"
    echo " Installing $val..."
    echo "---------------------------------------"
    cd $val
    bash 'setup_'$val'.sh'
    source 'setup_'$val'_vars.sh'
    cd ..
    echo "DONE!"
done
