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
THIS_SCRIPT="setup_bzlib.sh"

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

LIB_FILE=bzip2-1.0.8.tar.gz
LIB_FILE_PATH=$THIS_DIR/$LIB_FILE
LIB_NAME=bzip2-1.0.8
LIB_PATH=$THIS_DIR/$LIB_NAME
HOST_URL=https://gmap.pucrs.br
FILE_URL=$HOST_URL/public_data/spbench/libs/bzip2/$LIB_FILE

cd $THIS_DIR
	echo "Entering $THIS_DIR ..."

echo "Trying to run ./$THIS_DIR/$THIS_SCRIPT..."

TRIES=3

# Function to download the library file
download_file () {
	cd $THIS_DIR
	echo "Entering $THIS_DIR ..."
	if [ ! -f "$FILE" ]; then
		echo "Downloading $LIB_NAME..."
		wget -c --read-timeout=5 --tries=10 $FILE_URL
		if [ ! -f "$LIB_FILE" ]; then
			echo "Failed to download $LIB_NAME. Please check your internet connection and try again."
			echo "You can also try to download the file manually from $FILE_URL and place it inside $THIS_DIR."
			exit 1
		fi
	fi
}

libfile_exists () {
	cd $THIS_DIR
	echo "Entering $THIS_DIR ..."
	if [ ! -f "$LIB_FILE" ]; then
		echo "The library file $LIB_FILE was not found. Trying to download it first..."
		download_file
	fi
	return 0
}

extract_files () {
	cd $THIS_DIR
	echo "Entering $THIS_DIR ..."

	if [ $TRIES -eq 0 ]; then
		echo "Failed to install the library. Please check the error messages above."
	fi
	TRIES=$((TRIES-1))
	
	if libfile_exists; then
		echo "Library file $LIB_FILE found. Trying to decompress it..."
		echo "tar -xf $LIB_FILE..."
		# Extract the tar file and check if it was successful
		if tar -xf "$LIB_FILE"; then
			echo "Decompression of $LIB_FILE was successful."
		else
			echo "Decompression of $LIB_FILE failed. Trying to download it again..."
			download_file
			extract_files
		fi
	fi
}

libdir_exists () {
	cd $THIS_DIR
	echo "Entering $THIS_DIR ..."
	# Check if the library directory exists
	if [ -d "$LIB_PATH" ]; then
		# Check if the directory is not empty
		if [ ! "$(ls -A "$LIB_PATH")" ]; then
			echo "The library directory $LIB_PATH was not found or is empty. Trying to get the files..."
			extract_files
		fi
		return 1
	fi
	return 0
}

# Function to configure the library
configure_library () {
	libdir_exists
	cd $LIB_PATH
	echo "Entering $LIB_PATH ..."

	# Check if there is a Makefile
	if [ ! -f "Makefile" ]; then
		echo "Makefile not found. Trying to run the previous steps..."
		extract_files
	fi

	# Check if the build directory exists
	if [ ! -d "build" ]; then
		echo "The build directory does not exist. Creating it..."
		mkdir $LIB_PATH/build
	fi
}

# Function to build the library
build_library () {
	libdir_exists
	cd $LIB_PATH
	echo "Entering $LIB_PATH ..."
	echo "Building the library..."

	if make -j ; then
		echo "The library was built successfully."
	else
		echo "Failed to build the library."
	fi
}

# Function to install the library
install_library () {
	libdir_exists
	cd $LIB_PATH
	echo "Entering $LIB_PATH ..."
	# Check if the library has been built
	if [ ! -d "build" ]; then
		echo "The library has not been built yet. Building it first..."
		build_library
	fi
	cd $LIB_PATH
	echo "Entering $LIB_PATH ..."
	echo "Installing the library..."

	if make install PREFIX="$LIB_PATH/build" -j; then
		echo "The library was installed successfully."
	else
		echo "Failed to install the library."
		return 1
	fi
	return 0
}

# Function to prompt the user for input
prompt_user() {
    echo "Please select an option:"
    echo " 1) Do not reinstall (keep it as it is)"
    echo " 2) Download the library again, decompress the files, configure, build and install"
    echo " 3) Decompress already downloaded files, configure, build, and install"
    echo " 4) Only configure, build and install"
    echo "Enter your choice (1-4): "
    read choice
    case $choice in
        1)
            echo "Keeping the current installation."
			cd $THIS_DIR
	echo "Entering $THIS_DIR ..."
            ;;
        2)
			echo "Removing the current installation..."
			rm -rf $LIB_PATH

            download_file
            extract_files
			configure_library
            build_library
			if ! install_library; then
				cd $THIS_DIR
	echo "Entering $THIS_DIR ..."
				return 1
			fi
			;;
        3)
			echo "Removing the current installation..."
			rm -rf $LIB_PATH

            extract_files
			configure_library
            build_library
			if ! install_library; then
				cd $THIS_DIR
	echo "Entering $THIS_DIR ..."
				return 1
			fi
            ;;
        4)
			configure_library
            build_library
			if ! install_library; then
				cd $THIS_DIR
	echo "Entering $THIS_DIR ..."
				return 1
			fi
            ;;
        *)
			echo ""
            echo " ERROR: INVALID CHOICE!"
			echo ""
			sleep 1
			if ! prompt_user; then
				cd $THIS_DIR
	echo "Entering $THIS_DIR ..."
				return 1
			fi
            ;;
    esac
	return 0
}

# Check if the directory exists
if [ -d "$LIB_PATH" ]; then
    echo "A previous installation of the library exists."
	if ! prompt_user; then
		cd $THIS_DIR
	echo "Entering $THIS_DIR ..."
		return 1
	fi
else
    echo "No previous installation found. Proceeding with a fresh installation..."
    download_file
	extract_files
	configure_library
	build_library
	if ! install_library; then
		cd $THIS_DIR
	echo "Entering $THIS_DIR ..."
		return 1
	fi
fi

cd $THIS_DIR
	echo "Entering $THIS_DIR ..."
return 0