#!/usr/bin/env python3

## 
 ##############################################################################
 #  File  : install_libs.py
 #
 #  Title : Install the SPBench applications required libraries
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
import shutil
import runpy

from src.utils.dict import *
from src.utils.utils import *

def prompt_users():
    print("\nPlease select an option:")
    print(" 1) Proceed with the installation of the next libraries (WARNING: This may cause errors if the libraries depend on this one!).")
    print(" 2) Stop the libraries installation.")
    choice = input("Enter your choice: ")
    if choice == '1':
        print("Trying to install the remaining libraries...")
    elif choice == '2':
        print("Exiting the installation...")
        sys.exit(1)
    else:
        print("\n ERROR: INVALID CHOICE!\n")
        return 1
    return 0

def getEnvVar(var_name):
    if os.environ.get(var_name) is not None:
        return os.environ.get(var_name)
    return ""

def install_libraries(spbench_path, app_id):
    # Initialize logging if needed

    dependency_dict = getDependenciesRegistry(spbench_path)

    # Takes the app_id dependencies from the dictionary
    app_dependencies = dependency_dict[app_id]

    # Initialize a dependencies list with the length of the dependencies
    dependencies_list = [None] * len(app_dependencies)

    # Fill the dependencies list with the dependencies in the correct order in the list
    for key, val in app_dependencies.items():
        dependencies_list[int(key) - 1] = val

    global SPBENCH_LIBS_PATH
    # Store the original directory
    SPBENCH_LIBS_PATH = os.path.join(spbench_path, "libs")

    # If opencv is in the dependencies list, check if cmake is available in the system
    if "opencv" in dependencies_list:
        if shutil.which("cmake") is None:
            logging.error("SPBench could not find CMake. It is required to install OpenCV.")
            logging.error("Please, make sure CMake is installed and available in the PATH and try again.")
            if prompt_users() != 0:
                os.chdir(SPBENCH_LIBS_PATH)
                sys.exit(1)

    # Prepare the environment variables file
    env_vars_file = os.path.join(SPBENCH_LIBS_PATH, "libraries_env_vars.json")

    # Get the initial values of the environment variables
    ENV_LD_LYBRARY_PATH = getEnvVar("LD_LIBRARY_PATH")
    ENV_PATH = getEnvVar("PATH")
    ENV_CPATH = getEnvVar("CPATH")
    ENV_PKG_CONFIG_PATH = getEnvVar("PKG_CONFIG_PATH")

    # Save the original environment variables
    original_env_vars = {
        "LD_LIBRARY_PATH": ENV_LD_LYBRARY_PATH,
        "PATH": ENV_PATH,
        "CPATH": ENV_CPATH,
        "PKG_CONFIG_PATH": ENV_PKG_CONFIG_PATH
    }

    # Save the original environment variables to a file
    with open(env_vars_file, 'w') as f:
        json.dump(original_env_vars, f)

    for dependency in dependencies_list:

        print("---------------------------------------")
        print(f" Installing {dependency}...")
        print("---------------------------------------")

        dependency_dir = os.path.join(SPBENCH_LIBS_PATH, dependency)
        setup_script = os.path.join(dependency_dir, f"setup_{dependency}.py")

        # The shell script to source the setup script and print environment variables
        setup_vars_script = os.path.join(dependency_dir, f"setup_{dependency}_vars.sh")

        logging.info(f"Preparing {dependency}...")
        logging.info(f"Running > {sys.executable} {setup_script} {dependency_dir}")

        logging.info(f"Getting {dependency} data...")

        try:
            
            # Change to the dependency directory
            os.chdir(dependency_dir)

            # Save the original sys.argv
            original_argv = sys.argv

            # Set sys.argv to the required arguments for the script
            sys.argv = [setup_script, dependency_dir]

            # Run the setup script as a module
            runpy.run_path(setup_script, run_name="__main__")

            # Restore the original sys.argv
            sys.argv = original_argv

            # Change back to the original directory
            os.chdir(SPBENCH_LIBS_PATH)

        except Exception as e:
            logging.exception(f"Exception occurred while installing {dependency}.")
            if prompt_users() != 0:
                os.chdir(SPBENCH_LIBS_PATH)
                sys.exit(1)

        # Run the shell script and capture the output
        setup_vars_output = subprocess.run(f"bash {setup_vars_script}", capture_output=True, text=True, shell=True)

        # Parse the output to get environment variables
        env_vars = {}
        for line in setup_vars_output.stdout.splitlines():
            key, _, value = line.partition('=')
            env_vars[key] = value

        # Get the variables from the environment variables file
        env_vars_file_data = getDictFromJSON(env_vars_file)

        # Update the environment variables with the new values, separated by :
        for key, value in env_vars.items():
            if key in env_vars_file_data:
                env_vars_file_data[key] = f"{value.strip()}:{env_vars_file_data[key]}"
            else:
                env_vars_file_data[key] = value.strip()

        # Save the updated environment variables to the file
        with open(env_vars_file, 'w') as f:
            json.dump(env_vars_file_data, f)

        os.chdir(SPBENCH_LIBS_PATH)
        print("DONE!")