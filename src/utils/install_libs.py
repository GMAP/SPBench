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

import asyncio

from src.utils.dict import *
from src.utils.utils import *

def prompt_users():
    print("\nPlease select an option:")
    print(" 1) Proceed with the installation of the next libraries (WARNING: This may cause errors if the libraries depend on this one!).")
    print(" 2) Stop the libraries installation.")
    choice = input("Enter your choice: ")
    if choice == '1':
        logging.info("Trying to install the remaining libraries...")
    elif choice == '2':
        logging.info("Exiting the installation...")
        sys.exit(1)
    else:
        logging.error("\n ERROR: INVALID CHOICE!\n")
        return 1
    return 0

def getEnvVar(var_name):
    """Get the value of an environment variable."""
    if os.environ.get(var_name) is not None:
        return os.environ.get(var_name)
    return ""

def updateEnvVarFile(setup_vars_script, env_vars_file):
    """Update the environment variables file with the new values."""
    # Run the shell script and capture the output
    setup_vars_output = subprocess.run(f"bash {setup_vars_script}", capture_output=True, text=True, shell=True)

    subprocess.run(f"bash {setup_vars_script}", capture_output=True, text=True, shell=True)

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

def write_to_log_file(log_file, message):
    """Write the message to the log file."""
    with open(log_file, 'a') as f:
        f.write(message + '\n')

async def run_script(setup_script, log_file_path):
    """Run the setup script asynchronously and capture the output."""
    try:
        # Open the log file for writing
        with open(log_file_path, 'w', encoding='utf-8') as log_file:
            
            # Start the subprocess
            process = await asyncio.create_subprocess_exec(
                sys.executable, '-u', setup_script,
                stdout=asyncio.subprocess.PIPE,
                stderr=asyncio.subprocess.PIPE
            )

            async def read_stream(stream, log_prefix):
                """
                Reads a stream, logs its content, and prints it in real time.
                :param stream: The stream to read from.
                :param log_prefix: A prefix for the log entries (e.g., 'STDOUT', 'STDERR').
                """
                while True:
                    chunk = await stream.read(1024)  # Read in chunks of 1024 bytes
                    if not chunk:
                        break
                    decoded_chunk = chunk.decode('utf-8', errors='replace').strip()
                    log_file.write(f"{log_prefix}: {decoded_chunk}\n")  # Write to the log file
                    log_file.flush()  # Ensure content is written immediately
                    print(decoded_chunk)  # Print to console

            # Run both stdout and stderr reader tasks concurrently
            await asyncio.gather(
                read_stream(process.stdout, "STDOUT"),
                read_stream(process.stderr, "STDERR")
            )

            # Wait for the process to complete
            return_code = await process.wait()
            if return_code != 0:
                raise RuntimeError(f"Subprocess exited with error code {return_code}")
    
    except asyncio.exceptions.CancelledError:
        print("Process was cancelled.")

    except asyncio.CancelledError:
        print("Script execution was cancelled.")

    except RuntimeError as e:
        if str(e) == "Event loop is closed":
            print("The event loop has been closed.")
        else:
            logging.exception("Unexpected runtime error occurred.")
            print("An unexpected error occurred while running the script.")
            raise  # Re-raise the error if it's not the specific case we want to handle

    except Exception as e:
        logging.exception(f"An error occurred while running the script: {e}")
        print("An error occurred while running the script. Please check the logs for more details.")
    
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

        # Update the environment variables file with the new values
        updateEnvVarFile(setup_vars_script, env_vars_file)

        original_argv = sys.argv

        log_file_path = os.path.join(SPBENCH_LIBS_PATH, f"{dependency}/installation.log")

        try:
            # Change to the dependency directory
            os.chdir(dependency_dir)

            # Set sys.argv to the required arguments for the script
            sys.argv = [setup_script, dependency_dir]

            # Run the setup script
            asyncio.run(run_script(setup_script, log_file_path))

        except KeyboardInterrupt:
            print("Execution was interrupted.")
            sys.exit(1)
        except RuntimeError as e:
            if str(e) == "Event loop is closed":
                print("The event loop has been closed.")
            else:
                logging.exception("Unexpected runtime error occurred.")
                raise
        except Exception as e:
            logging.exception(f"Exception occurred while installing {dependency}: {e}")
            if prompt_users() != 0:
                os.chdir(SPBENCH_LIBS_PATH)
                sys.exit(1)

        finally:
            # Restore the original sys.argv
            sys.argv = original_argv

        os.chdir(SPBENCH_LIBS_PATH)
        print("DONE!")