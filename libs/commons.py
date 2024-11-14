#!/usr/bin/env python3

## 
 ##############################################################################
 #  File  : commons.py
 #
 #  Title : Common functions for the library installation scripts
 #
 #  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 #
 #  Date  : November 14, 2024
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

import urllib.request
import logging
import sys
import os
import tarfile
import shutil
import json
import subprocess

num_tries = 3

def get_script_dir():
    """Function to determine the directory of the currently executed or sourced script."""
    script = os.path.realpath(sys.argv[0])
    script_dir = os.path.dirname(script)
    return script_dir

def download_file(lib_name, file_url, abs_lib_file_path):
    try: 
        logging.info(f"Downloading {lib_name} from {file_url}...")
        urllib.request.urlretrieve(file_url, abs_lib_file_path, reporthook=progress_hook)
        logging.info(f"Download of {lib_name} completed.")
    
    except urllib.error.URLError as e:
        logging.error(f"Failed to download {lib_name}: {e}")
        sys.exit(1)
    
    except Exception as e:
        logging.error(f"Error during download process: {e}")
        sys.exit(1)
    

def progress_hook(block_num, block_size, total_size):
    downloaded = block_num * block_size
    if total_size > 0:
        percent = downloaded / total_size
        bar_length = 40  # Length of the progress bar
        bar = '=' * int(bar_length * percent) + '-' * (bar_length - int(bar_length * percent))
        progress_message = f"\rDownloading the library: |{bar}| {percent * 100:.1f}%"
        sys.stdout.write(progress_message)
        sys.stdout.flush()  # Ensure the message is printed immediately
    else:
        sys.stdout.write(f"\rDownloading the library: {downloaded} bytes")
        sys.stdout.flush()

def setup_logging():
    """Configure logging for the script."""
    logging.basicConfig(
        level=logging.INFO,  # Set logging level to INFO
        format='%(asctime)s | %(levelname)s: %(message)s',  # Define log message format
        datefmt='%Y-%m-%d %H:%M:%S',  # Define date format
        #handlers=[logging.StreamHandler()],  # Output logs to console
        #format='%(message)s',  # Define log message format
        stream=sys.stdout, 
        force=True,
    )

def libfile_exists(lib_tar_file, abs_lib_file_path):
    try:
        if not os.path.isfile(abs_lib_file_path):
            logging.info(f"The library file {lib_tar_file} was not found. Trying to download it first...")
            download_file()
            
            # Check again after attempting download
            if not os.path.isfile(abs_lib_file_path):
                logging.error(f"Failed to download {lib_tar_file}.")
                return False
        
        logging.info(f"Found library file {lib_tar_file} at {abs_lib_file_path}")
        return True
    
    except Exception as e:
        logging.error(f"Error checking or downloading library file: {e}")
        return False

def extract_files(lib_tar_file, abs_lib_file_path, lib_root_dir):
    try:
        global num_tries
        
        while num_tries > 0:
            if not libfile_exists(lib_tar_file, abs_lib_file_path):
                num_tries -= 1
                logging.error(f"Attempt {4 - num_tries}: Library file {lib_tar_file} not found.")
                if num_tries == 0:
                    logging.error("Failed to install the library after all attempts.")
                    sys.exit(1)
                logging.info(f"Trying to download {lib_tar_file} again...")
                download_file()
            else:
                logging.info(f"Library file {lib_tar_file} found. Attempting to decompress it...")
                try:
                    with tarfile.open(abs_lib_file_path, 'r:gz') as tar:
                        for member in tar.getmembers():
                            logging.info(f"Extracting {member.name}...")
                            tar.extract(member, path=lib_root_dir)
                    logging.info(f"Decompression of {lib_tar_file} was successful.")
                    return
                except tarfile.TarError:
                    logging.error(f"Decompression of {lib_tar_file} failed.")
                    num_tries -= 1
                    if num_tries > 0:
                        logging.info(f"Retrying... {num_tries} tries left.")
                    else:
                        logging.error("Failed to install the library after all attempts.")
                        sys.exit(1)
    
    except Exception as e:
        logging.error(f"Error during extraction process: {e}")
        sys.exit(1)


def libdir_exists(lib_extracting_path):
    try:
        # Check if the library directory exists
        if os.path.isdir(lib_extracting_path):
            # Check if the directory is not empty
            if not os.listdir(lib_extracting_path):
                logging.info(f"The library directory {lib_extracting_path} was found but is empty. Trying to get the files...")
                extract_files()
            return True
        
        logging.info(f"The library directory {lib_extracting_path} was not found. Trying to get the files...")
        extract_files()
        return True
    
    except Exception as e:
        logging.error(f"Error checking or accessing library directory: {e}")
        return False

def remove_directory(dir_path):
    try:
        logging.info(f"Removing directory: {dir_path}")
        shutil.rmtree(dir_path)
        logging.info(f"Successfully removed directory: {dir_path}")
    except Exception as e:
        logging.error(f"Error removing directory {dir_path}: {e}")
        sys.exit(1)

def update_global_env():
    env_vars_file = os.path.join(get_script_dir(), "../libraries_env_vars.json")

    logging.info(f"Reading environment variables from {env_vars_file}...")

    # Read the environment variables from the JSON file
    with open(env_vars_file) as f:
        env_vars = json.load(f)

    # Combine them with the current environment variables
    lib_env_vars = os.environ.copy()
    lib_env_vars.update(env_vars)
    return lib_env_vars

# Function to build the libraryrary
def run_sh_commands(configure_command, lib_env_vars=''):
    try:
        process = subprocess.run(configure_command, env=lib_env_vars)
        if process.returncode == 0:
            logging.info("Running: " + configure_command)
        else:
            logging.error("Failed to run the command: " + configure_command)
    except subprocess.CalledProcessError as e:
        logging.error(f"Execution failed with error: {e}")

# Function to build the libraryrary
def run_configure_command(configure_command, lib_env_vars):
    try:
        process = subprocess.run(configure_command, env=lib_env_vars)
        if process.returncode == 0:
            logging.info("Configuration was successful.")
        else:
            logging.error("Configuration failed.")
    except subprocess.CalledProcessError as e:
        logging.error(f"Configuration failed with error: {e}")
        sys.exit(1)

def run_build_command(build_command, lib_env_vars):
    try:
        process = subprocess.run(build_command, env=lib_env_vars)
        if process.returncode == 0:
            logging.info("The library was built successfully.")
        else:
            logging.error("Building process has failed.")
    except subprocess.CalledProcessError as e:
        logging.error(f"Failed to build the library: {e}")
        sys.exit(1)

def run_install_command(install_command, lib_env_vars):
    try:
        process = subprocess.run(install_command, env=lib_env_vars)
        if process.returncode == 0:
            logging.info("The library was installed successfully.")
            return True
        else:
            logging.info(f"Failed to install the library (return code: {process.returncode}).")
            return False
    except Exception as e:
        logging.error(f"Error during installation: {e}")
        return False