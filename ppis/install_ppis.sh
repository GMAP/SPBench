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

echo "---------------------------------------"
echo " Installing FastFlow..."
echo "---------------------------------------"
cd fastflow
sh setup_fastflow.sh
cd ..
echo "DONE!"

echo "---------------------------------------"
echo " Installing SPAR..."
echo "---------------------------------------"
cd SPar
sh setup_spar.sh
make
cd ..
echo "DONE!"

echo "---------------------------------------"
echo " Installing TBB..."
echo "---------------------------------------"
cd tbb
sh setup_tbb.sh
cd ..
echo "DONE!"

echo "---------------------------------------"
echo " Installing GrPPI..."
echo "---------------------------------------"
cd grppi
sh setup_grppi.sh
cd ..
echo "DONE!"

echo "---------------------------------------"
echo " Installing WindFlow..."
echo "---------------------------------------"
cd WindFlow
sh setup_windflow.sh
cd ..
echo "DONE!"