## 
 ##############################################################################
 #  File  : utils.py
 #
 #  Title : SPBench makefile generator
 #
 #  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 #
 #  Date  : July 06, 2021
 #
 #  Copyright (C) 2021 Adriano M. Garcia
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
import math
import logging

from .dict import *

spbench_path = os.path.abspath(os.path.dirname(os.path.realpath(__file__)) + "/../../")

from sys import version_info 
python_3 = version_info[0]

class color:
    PURPLE = '\033[95m'
    CYAN = '\033[96m'
    DARKCYAN = '\033[36m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    END = '\033[0m'

# list of SPBench reserved words
reserved_words = {'all', 'bzip2', 'ferret', 'lane_detection', 'person_recognition', 'source', 'sink', 'spbench'}

# check if string is blank
def isNotBlank (my_string):
    return bool(my_string and my_string.strip())

def variance(data, ddof=0):
	n = len(data)
	mean = sum(data) / n
	return sum((x - mean) ** 2 for x in data) / (n - ddof)

def stdev(data):
	var = variance(data)
	std_dev = math.sqrt(var)
	return std_dev

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

# function that receives a application name and check if its dependencies are listed in the libs/libraries_env_vars.json file, and return a list of missing dependencies
def checkDependencies(app_name):
    """
    Check if the dependencies of a given application are listed in the libraries_env_vars.json file

    Args:
        app_name (str): The name of the application

    Returns:
        list: A list of missing dependencies
    """

    env_vars_file = spbench_path + "/libs/libraries_env_vars.json"
    dependencies_file = spbench_path + "/libs/dependencies.json"

    # Read the dependencies from the JSON file
    with open(dependencies_file) as f:
        dependencies = json.load(f)

    # get the dependencies of the given application
    app_dependencies = list(dependencies[app_name].values())

    print(f"Dependencies: {app_dependencies}")

    # Read the environment variables from the JSON file if the file exists and is not empty
    if not fileExists(env_vars_file):
        return app_dependencies
    
    with open(env_vars_file) as f:
        env_vars = json.load(f)

    print(f"Environment variables: {env_vars}")

    # return a list of dependencies that are not in the environment variables
    for dependency in dependencies:
        for key, value in env_vars.items():
            if dependency in value:
                app_dependencies.remove(dependency)
                
    print(f"Missing dependencies: {app_dependencies}")

    return app_dependencies

# function that receives a library name and add the environment variables to the libs/libraries_env_vars.json file
def addEnvVars(app_id):
    """
    Add the environment variables of an application to the libraries_env_vars.json file

    Args:
        app_id (str): The name of the base application
    """

    dependencies = getDependenciesRegistry(spbench_path)[app_id]
    env_vars_file = spbench_path + "/libs/libraries_env_vars.json"

    for lib_name in dependencies.values():

        env_vars_script = spbench_path + "/libs/" + lib_name + f"/setup_{lib_name}_vars.sh"

        # Run the shell script and capture the output
        setup_vars_output = subprocess.run(f"bash {env_vars_script}", capture_output=True, text=True, shell=True)

        # Parse the output to get environment variables
        env_vars = {}
        for line in setup_vars_output.stdout.splitlines():
            key, _, value = line.partition('=')
            env_vars[key] = value

        env_vars_dict = getEnvVarsJSON() # initialize the env_vars_file_data variable with an empty dictionary

        # Update the environment variables with the new values, separated by :
        for key, value in env_vars.items():
            # Check if the key already exists in the file
            if key in env_vars_dict:
                # check if the dependency string is already on the value of the env var on the file and add it if it is not
                if value.strip() not in env_vars_dict[key]:
                    env_vars_dict[key] = f"{value.strip()}:{env_vars_dict[key]}"
            # if the key does not exist, add it to the file
            else:
                env_vars_dict[key] = value.strip()

        # Save the updated environment variables to the file
        with open(env_vars_file, 'w') as f:
            json.dump(env_vars_dict, f, indent=4)



# get environment variables from the libs/libraries_env_vars.json file
def getEnvVarsJSON():
    """
    Get the environment variables from the libraries_env_vars.json file

    Returns:
        dict: A dictionary with the environment variables
        Or None if the file does not exist
    """

    env_vars_file = spbench_path + "/libs/libraries_env_vars.json"

    # check if the file exists and is a valid json file
    if not fileExists(env_vars_file): return {}
    
    # check if the file is empty
    if os.stat(env_vars_file).st_size == 0: return {}
    
    # Read the environment variables from the JSON file
    with open(env_vars_file) as f:
        env_vars = json.load(f)

    return env_vars
 
def checkEnvVarsJSON():
    """
    Check if the environment variables file exists and is not empty
    """
    env_vars_file = spbench_path + "/libs/libraries_env_vars.json"
    # check if the file exists and is a valid json file
    if not fileExists(env_vars_file):
        print("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
        logging.warning(" Environment variables file not found:\n " + env_vars_file)
        logging.warning(" If your application depends on a library, this may cause errors.")
        logging.warning(" Please run the install command again and select the appropriate options.")
        logging.warning(" If you are loading the libraries manually, you can ignore this message.")
        print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n")
        return

    # check if the file is empty
    if os.stat(env_vars_file).st_size == 0:
        print("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
        logging.warning(" No environment variables set on:\n " + env_vars_file)
        logging.warning(" If your application depends on a library, this may cause errors.")
        logging.warning(" Please run the setup script again to generate the environment variables.")
        logging.warning(" If you are loading the libraries manually, you can ignore this message.")
        print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n")

    return