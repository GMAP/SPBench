#!/usr/bin/env python3

## 
 ##############################################################################
 #  File  : setup_opencv.py
 #
 #  Title : Install the OpenCV Library
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
import subprocess
import tarfile
import logging
import shutil
import urllib.request
import json
import select

THIS_SCRIPT = "setup_opencv.sh"

LIB_FILE = "opencv-2.4.13.6.tar.gz"
LIB_NAME = "opencv-2.4.13.6"
HOST_URL = "https://gmap.pucrs.br"
FILE_URL = f"{HOST_URL}/public_data/spbench/libs/opencv/{LIB_FILE}"
num_tries = 3

def setup_logging():
    """Configure logging for the script."""
    logging.basicConfig(
        level=logging.INFO,  # Set logging level to INFO
        #format='%(asctime)s - %(levelname)s - %(message)s',  # Define log message format
        #datefmt='%Y-%m-%d %H:%M:%S',  # Define date format
        handlers=[logging.StreamHandler()]  # Output logs to console
    )

def get_log_level(line):
    warning_keywords = ["warning", "deprecated", "note"]
    error_keywords = ["error", "fatal", "failed"]
    if any(keyword in line.lower() for keyword in error_keywords):
        return logging.ERROR
    elif any(keyword in line.lower() for keyword in warning_keywords):
        return logging.WARNING
    else:
        return logging.INFO
    
def get_script_dir():
    """Function to determine the directory of the currently executed or sourced script."""
    script = os.path.realpath(sys.argv[0])
    script_dir = os.path.dirname(script)
    return script_dir

def check_script_exists(script_dir, script_name):
    """Function to check if the specified script exists in the given directory."""
    script_path = os.path.join(script_dir, script_name)
    if not os.path.isfile(script_path):
        logging.error(f"The script {script_path} was not found.")
        logging.error("Please make sure you are running this script from the same directory where the script is, or pass this script path as an argument.")
        sys.exit(1)
    return script_path

def download_file():
    try:
        original_dir = os.getcwd()  # Store original directory
        os.chdir(THIS_DIR)
        
        logging.info(f"Downloading {LIB_NAME} from {FILE_URL}...")
        urllib.request.urlretrieve(FILE_URL, LIB_FILE_PATH, reporthook=progress_hook)
        logging.info(f"Download of {LIB_NAME} completed.")
    
    except urllib.error.URLError as e:
        logging.error(f"Failed to download {LIB_NAME}: {e}")
        sys.exit(1)
    
    except Exception as e:
        logging.error(f"Error during download process: {e}")
        sys.exit(1)
    
    finally:
        os.chdir(original_dir)  # Return to original directory

def progress_hook(blocknum, blocksize, totalsize):
    """Function to display download progress."""
    percent = int(blocknum * blocksize * 100 / totalsize)
    sys.stdout.write(f"\rDownloading {LIB_NAME}: {percent}%")
    sys.stdout.flush()

def libfile_exists():
    try:
        original_dir = os.getcwd()  # Store original directory
        os.chdir(THIS_DIR)
        
        if not os.path.isfile(LIB_FILE_PATH):
            logging.info(f"The library file {LIB_FILE} was not found. Trying to download it first...")
            download_file()
            
            # Check again after attempting download
            if not os.path.isfile(LIB_FILE_PATH):
                logging.error(f"Failed to download {LIB_FILE}.")
                return False
        
        logging.info(f"Found library file {LIB_FILE} at {LIB_FILE_PATH}")
        return True
    
    except Exception as e:
        logging.error(f"Error checking or downloading library file: {e}")
        return False
    
    finally:
        os.chdir(original_dir)  # Return to original directory

def extract_files():
    try:
        original_dir = os.getcwd()  # Store original directory
        os.chdir(THIS_DIR)
        
        global num_tries
        
        while num_tries > 0:
            if not libfile_exists():
                num_tries -= 1
                logging.info(f"Attempt {4 - num_tries}: Library file {LIB_FILE} not found.")
                if num_tries == 0:
                    logging.error("Failed to install the library after all attempts.")
                    sys.exit(1)
                logging.info(f"Trying to download {LIB_FILE} again...")
                download_file()
            else:
                logging.info(f"Library file {LIB_FILE} found. Attempting to decompress it...")
                try:
                    with tarfile.open(LIB_FILE_PATH, 'r:gz') as tar:
                        for member in tar.getmembers():
                            logging.info(f"Extracting {member.name}...")
                            tar.extract(member, path=THIS_DIR)
                    logging.info(f"Decompression of {LIB_FILE} was successful.")
                    return
                except tarfile.TarError:
                    logging.error(f"Decompression of {LIB_FILE} failed.")
                    num_tries -= 1
                    if num_tries > 0:
                        logging.info(f"Retrying... {num_tries} tries left.")
                    else:
                        logging.error("Failed to install the library after all attempts.")
                        sys.exit(1)
    
    except Exception as e:
        logging.error(f"Error during extraction process: {e}")
        sys.exit(1)
    
    finally:
        os.chdir(original_dir)  # Return to original directory

def libdir_exists():
    try:
        original_dir = os.getcwd()  # Store original directory
        os.chdir(THIS_DIR)
        
        # Check if the library directory exists
        if os.path.isdir(LIB_PATH):
            # Check if the directory is not empty
            if not os.listdir(LIB_PATH):
                logging.info(f"The library directory {LIB_PATH} was found but is empty. Trying to get the files...")
                extract_files()
            return True
        
        logging.info(f"The library directory {LIB_PATH} was not found. Trying to get the files...")
        extract_files()
        return True
    
    except Exception as e:
        logging.error(f"Error checking or accessing library directory: {e}")
        return False
    
    finally:
        os.chdir(original_dir)  # Return to original directory


def run_stuff(run_this):

    # Configure logging
    logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

    # Use the provided environment variables or the current environment
    env = LIB_ENV_VARS
    
    # Start the subprocess
    process = subprocess.Popen(run_this, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, env=env)

    # Function to determine the log level based on the output line
    def get_log_level(line):
        warning_keywords = ["warning", "deprecated", "note"]
        error_keywords = ["error", "fatal", "failed"]
        if any(keyword in line.lower() for keyword in error_keywords):
            return logging.ERROR
        elif any(keyword in line.lower() for keyword in warning_keywords):
            return logging.WARNING
        else:
            return logging.INFO

    try:
        # Read stdout and stderr in real time
        while True:
            stdout_line = process.stdout.readline().strip()
            stderr_line = process.stderr.readline().strip()

            if stdout_line:
                logging.info(stdout_line)
                print(stdout_line)
            if stderr_line:
                log_level = get_log_level(stderr_line)
                logging.log(log_level, stderr_line)
                print(stderr_line, file=sys.stderr)

            # Check if the process has terminated
            if process.poll() is not None:
                break

        # Capture any remaining output
        remaining_stdout, remaining_stderr = process.communicate()

        if remaining_stdout:
            logging.info(remaining_stdout.strip())
            print(remaining_stdout)
        if remaining_stderr:
            log_level = get_log_level(remaining_stderr.strip())
            logging.log(log_level, remaining_stderr.strip())
            print(remaining_stderr, file=sys.stderr)

    except Exception as e:
        logging.error(f"Error reading output: {e}")

    finally:
        # Wait for the process to complete and get the return code
        process.wait()



    return process


def configure_library():
    
    if libdir_exists():
        original_dir = os.getcwd()
        os.chdir(LIB_PATH)

        # Check if the library has a CMakeLists.txt file
        if not os.path.isfile("CMakeLists.txt"):
            logging.info("The library does not have a configure file. Trying to run the previous steps...")
            extract_files()

        # Check if there is a cmake directory
        if not os.path.isdir("cmake"):
            logging.info("CMake files not found. Trying to run the previous steps...")
            extract_files()

        # Create a fresh build directory
        build_dir = os.path.join(LIB_PATH, "build")
        if os.path.isdir(build_dir):
            logging.info("Cleanning previous builds...")
            shutil.rmtree(build_dir)
        os.mkdir(build_dir)

        #prefix = os.path.join(LIB_PATH, "build")

        logging.info("Configuring the library...")
        os.chdir(build_dir)
        cmake_command = [
            "cmake",
            "-DBUILD_PNG=ON",
            "-DCMAKE_CXX_FLAGS=-std=c++1y",
            "-DWITH_FFMPEG=ON",
            "-DOPENCV_FFMPEG_SKIP_BUILD_CHECK=ON",
            "-DWITH_CUDA=OFF",
            "-DCMAKE_INSTALL_PREFIX=../",
            "-DOPENCV_GENERATE_PKGCONFIG=ON",
            "-DBUILD_TESTS=OFF",
            "-DBUILD_EXAMPLES=OFF",
            "-DBUILD_PERF_TESTS=OFF",
            "-DBUILD_EXAMPLES=OFF",
            "-DBUILD_opencv_apps=OFF",
            ".."
        ]
        try:
            # Construct the shell command to run
            command = ["sh", "-c", f"{' '.join(cmake_command)}"]
            process = subprocess.run(cmake_command, env=LIB_ENV_VARS)
            if process.returncode == 0:
                logging.info("Configuration was successful.")
            else:
                logging.error("Configuration failed.")
        except subprocess.CalledProcessError as e:
            logging.error(f"Configuration failed with error: {e}")
        finally:
            os.chdir(original_dir)

def build_library():
    if libdir_exists():
        original_dir = os.getcwd()
        os.chdir(LIB_PATH)
        logging.info("Building the library...")
        
        # Check if there is a Makefile
        build_dir = os.path.join(LIB_PATH, "build")
        os.chdir(build_dir)
        if not any(f.startswith("Makefile") for f in os.listdir(build_dir)):
            logging.info("Makefile not found. Trying to run the previous steps...")
            extract_files()
            configure_library()

        try:
            # Command to be executed
            command = ["sh", "-c", "make -j"]
            # Run the command
            process = subprocess.run(command, env=LIB_ENV_VARS)
            if process.returncode == 0:
                logging.info("The library was built successfully.")
            else:
                logging.error("Building process has failed.")
        except subprocess.CalledProcessError as e:
            logging.error(f"Failed to build the library: {e}")
        finally:
            os.chdir(original_dir)

# Function to install the library
def install_library():
    if libdir_exists():
        os.chdir(LIB_PATH)
        # Check if the library has been built
        if not os.path.isdir("build"):
            print("The library has not been built yet. Building it first...")
            build_library()

        build_dir = os.path.join(LIB_PATH, "build")
        os.chdir(build_dir)
        print("Installing the library...")

        try:
            # Command to be executed
            command = ["sh", "-c", "make install -j"]
            # Run the command
            process = subprocess.run(command, env=LIB_ENV_VARS)
            # Check the return code
            if process.returncode == 0:
                print("The library was installed successfully.")
                return True
            else:
                print(f"Failed to install the library (return code: {process.returncode}).")
                return False
        except Exception as e:
            print(f"Error during installation: {e}")
            return False

def remove_directory(dir_path):
    try:
        logging.info(f"Removing directory: {dir_path}")
        shutil.rmtree(dir_path)
        logging.info(f"Successfully removed directory: {dir_path}")
    except Exception as e:
        logging.error(f"Error removing directory {dir_path}: {e}")
        sys.exit(1)

# Function to prompt the user for input
def prompt_user():
    print("Please select an option:")
    print(" 1) Do not reinstall (keep it as it is)")
    print(" 2) Download the library again, decompress the files, configure, build and install")
    print(" 3) Decompress already downloaded files, configure, build, and install")
    print(" 4) Only configure, build and install")
    choice = input("Enter your choice (1-4): ")
    if choice == "1":
        logging.info("Keeping the current installation.")
    elif choice == "2":
        logging.info("Removing the current installation...")
        remove_directory(LIB_PATH)
        download_file()
        extract_files()
        configure_library()
        build_library()
        if not install_library():
            return False
    elif choice == "3":
        logging.info("Removing the current installation...")
        remove_directory(LIB_PATH)
        extract_files()
        configure_library()
        build_library()
        if not install_library():
            return False
    elif choice == "4":
        configure_library()
        build_library()
        if not install_library():
            return False
    else:
        logging.error("\nERROR: INVALID CHOICE!\n")
        if not prompt_user():
            return False
    return True

def main():

    setup_logging()

    global THIS_DIR
    global LIB_FILE_PATH
    global LIB_PATH

    if len(sys.argv) < 2:
        logging.info("The directory of the currently executed script was not passed as an argument.")
        logging.info("Trying to determine the directory of the currently executed script...")
        THIS_DIR = get_script_dir()
    else:
        THIS_DIR = sys.argv[1]


    # Check if the specified script exists in the provided directory
    script_path = check_script_exists(THIS_DIR, THIS_SCRIPT)
    
    # Proceed with script execution or further operations
    logging.info(f"Script found: {script_path}")

    logging.info(f"Trying to run {os.path.join(THIS_DIR, THIS_SCRIPT)}...")

    LIB_FILE_PATH = os.path.join(THIS_DIR, LIB_FILE)
    LIB_PATH = os.path.join(THIS_DIR, LIB_NAME)

    os.chdir(THIS_DIR)

    global LIB_ENV_VARS
    ENV_VARS_FILE = os.path.join(THIS_DIR, "../libraries_env_vars.json")

    # Read the environment variables from the JSON file
    with open(ENV_VARS_FILE) as f:
        env_vars = json.load(f)

    # Combine them with the current environment variables
    LIB_ENV_VARS = os.environ.copy()
    LIB_ENV_VARS.update(env_vars)

    # Check if the directory exists
    if os.path.isdir(LIB_PATH):
        logging.info("A previous installation of the library exists.")
        if not prompt_user():
            sys.exit(1)
    else:
        logging.info("No previous installation found. Proceeding with a fresh installation...")
        download_file()
        extract_files()
        configure_library()
        build_library()
        if not install_library():
            sys.exit(1)

    os.chdir(THIS_DIR)

if __name__ == "__main__":
    main()