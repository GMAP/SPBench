#!/usr/bin/env python3

## 
 ##############################################################################
 #  File  : setup_gsl.py
 #
 #  Title : Install the GSL Library
 #
 #  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 #
 #  Date  : July 03, 2024
 #
 #  Copyright (C) 2024 Adriano M. Garcia
 # 
 #  This program is free software: you can redistribute it and/or modify
 #  it under the terms of the GNU General Public License as published by
 #  the Free Software Foundation, either version 3 of the License, or
 #  (at your option) any later version.
 #
 #  This program is distributed in the hope that it will be useful,
 #  but WITHOUT ANY WARRANTY; without even the implied warranty of
 #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 #  GNU General Public License for more details.
 #
 #  You should have received a copy of the GNU General Public License
 #  along with this program. If not, see <https://www.gnu.org/licenses/>.
 # 
 ##############################################################################
##

import os
import sys
import logging

sys.path.insert(1, os.path.join(sys.path[0], '..'))
import commons

THIS_SCRIPT = "setup_gsl.sh"

TAR_LIB_FILE = "gsl-2.6.tar.gz"
LIB_NAME = "gsl-2.6"
HOST_URL = "https://gmap.pucrs.br"
FILE_URL = f"{HOST_URL}/public_data/spbench/libs/gsl/{TAR_LIB_FILE}"

def get_script_dir():
    """Function to determine the directory of the currently executed or sourced script."""
    script = os.path.realpath(sys.argv[0])
    script_dir = os.path.dirname(script)
    return script_dir

def configure_library():
    if commons.libdir_exists(LIB_EXTRACTING_PATH):
        original_dir = os.getcwd()
        os.chdir(LIB_EXTRACTING_PATH)
        
        # Check if the library has a configure file
        if not os.path.isfile("configure"):
            logging.info("The library does not have a configure file. Trying to run the previous steps...")
            commons.extract_files(TAR_LIB_FILE, ABS_LIB_FILE_PATH, THIS_DIR)
        
        # Create a fresh build directory
        build_dir = os.path.join(LIB_EXTRACTING_PATH, "build")
        if os.path.isdir(build_dir):
            logging.info("Cleanning previous builds...")
            commons.remove_directory(build_dir)
        os.mkdir(build_dir)
        
        os.chdir(LIB_EXTRACTING_PATH)
        PREFIX = os.path.join(os.getcwd(), "build")
        os.environ["PATH"] = f"{PREFIX}/bin:{os.environ['PATH']}"
        os.environ["PKG_CONFIG_PATH"] = f"{PREFIX}/pkgconfig"
        logging.info("Configuring the library...")

        configure_path = os.path.join(LIB_EXTRACTING_PATH, "configure")

        configure_command = ["sh", "-c", f"{configure_path} --prefix={PREFIX} --enable-shared"]

        commons.run_configure_command(configure_command, LIB_ENV_VARS)
        
        os.chdir(original_dir)

def build_library():
    if commons.libdir_exists(LIB_EXTRACTING_PATH):
        original_dir = os.getcwd()
        os.chdir(LIB_EXTRACTING_PATH)
        logging.info("Building the library...")
        
        # Check if there is a Makefile
        if not any(f.startswith("Makefile") for f in os.listdir(LIB_EXTRACTING_PATH)):
            logging.info("Makefile not found. Trying to run the previous steps...")
            commons.extract_files(TAR_LIB_FILE, ABS_LIB_FILE_PATH, THIS_DIR)
            configure_library()

        #os.chdir(THIS_DIR)
        
        build_command = ["sh", "-c", f"make -C {LIB_EXTRACTING_PATH} -j"]
        #build_command = ["sh", "-c", f"{THIS_DIR}/run_build.sh {LIB_EXTRACTING_PATH}"]

        commons.run_build_command(build_command, LIB_ENV_VARS)

        os.chdir(original_dir)   

# Function to install the library
def install_library():
    if commons.libdir_exists(LIB_EXTRACTING_PATH):
        os.chdir(LIB_EXTRACTING_PATH)
        # Check if the library has been built
        if not os.path.isdir("build"):
            logging.info("The library has not been built yet. Building it first...")
            build_library()
        os.chdir(LIB_EXTRACTING_PATH)
        logging.info("Installing the library...")

        install_command = ["sh", "-c", f"make install -C {LIB_EXTRACTING_PATH} -j"]

        return commons.run_install_command(install_command, LIB_ENV_VARS)
    else:
        logging.error(f"The library directory was not found: {LIB_EXTRACTING_PATH}")
        return False

# Function to prompt the user for input
def prompt_user():
    print(f"\nThere is a previous instalation of {LIB_NAME}.")
    print("What would you like to do?\n")
    print("1) Do not reinstall the library (keep it as it is)")
    print("2) Download the library again, decompress the files, configure, build, and install")
    print("3) Decompress already downloaded files, configure, build, and install")
    print("4) Only configure, build, and install")
    print("5) Only load the library")
    print("\nEnter your choice (1-4): ")
    choice = input("")
    if choice == "1":
        logging.info("Keeping the current installation.")
    elif choice == "2":
        logging.info("Removing the current installation...")
        commons.remove_directory(LIB_EXTRACTING_PATH)
        commons.download_file(LIB_NAME, FILE_URL, ABS_LIB_FILE_PATH)
        commons.extract_files(TAR_LIB_FILE, ABS_LIB_FILE_PATH, THIS_DIR)
        configure_library()
        build_library()
        if not install_library():
            return False
    elif choice == "3":
        logging.info("Removing the current installation...")
        commons.remove_directory(LIB_EXTRACTING_PATH)
        commons.extract_files(TAR_LIB_FILE, ABS_LIB_FILE_PATH, THIS_DIR)
        configure_library()
        build_library()
        if not install_library():
            return False
    elif choice == "4":
        configure_library()
        build_library()
        if not install_library():
            return False
    elif choice == "5":
        logging.info("Library loaded!")
    else:
        logging.error("\nERROR: INVALID CHOICE!\n")
        if not prompt_user():
            return False
    return True

def main():

    commons.setup_logging()

    global THIS_DIR
    global ABS_LIB_FILE_PATH
    global LIB_EXTRACTING_PATH

    if len(sys.argv) < 2:
        logging.info("The directory of the currently executed script was not passed as an argument.")
        logging.info("Trying to determine the directory of the currently executed script...")
        THIS_DIR = get_script_dir()
    else:
        THIS_DIR = sys.argv[1]

    logging.info(f"Trying to run {os.path.join(THIS_DIR, THIS_SCRIPT)}...")

    ABS_LIB_FILE_PATH = os.path.join(THIS_DIR, TAR_LIB_FILE)
    LIB_EXTRACTING_PATH = os.path.join(THIS_DIR, LIB_NAME)

    os.chdir(THIS_DIR)

    global LIB_ENV_VARS 
    LIB_ENV_VARS = commons.update_global_env()

    # Check if the directory exists
    if os.path.isdir(LIB_EXTRACTING_PATH):
        print("")
        logging.warning("A previous installation of the library exists.")
        if not prompt_user():
            sys.exit(1)
    else:
        logging.info("No previous installation found. Proceeding with a fresh installation...")
        commons.download_file(LIB_NAME, FILE_URL, ABS_LIB_FILE_PATH)
        commons.extract_files(TAR_LIB_FILE, ABS_LIB_FILE_PATH, THIS_DIR)
        configure_library()
        build_library()
        if not install_library():
            sys.exit(1)

    os.chdir(THIS_DIR)

if __name__ == "__main__":
    main()