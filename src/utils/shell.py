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

import hashlib
import sys
import os
import subprocess
import json

from . import utils

spbench_path = os.path.abspath(os.path.dirname(os.path.realpath(__file__)) + "/../../")

def programmExists(programm, err_msg):
    """check if a given programm exists in shell
    """
    check_programm_cmd = "command -v " + programm + " >/dev/null 2>&1 || { echo >&2 \" " + err_msg + "\"; exit 1; }"

    if(utils.python_3 == 3):
        try:
            retcode = subprocess.call(check_programm_cmd, shell=True)
            if -retcode < 0:
                return False
        except OSError as e:
            print(" Execution failed:", e, file=sys.stderr)
            print()
        except KeyboardInterrupt as e:
            print(" KeyboardInterrupt")
            print("\n Unsuccessful execution\n")
            sys.exit()
    return True

def getTextEditor(user_editor):
    text_editor = ''
    # if no editor was provided by the user, try to find one
    if not user_editor:
        if programmExists("nano", ''):
            text_editor = 'nano'
        elif programmExists("vim", ''):
            text_editor = 'vim'
        elif programmExists("vi", ''):
            text_editor = 'vi'
        else:
            print("\n No text editor found! Please provide one through the \'-editor\' argument.")
            print(" Type ./spbench [command] -help for more info.\n")
            sys.exit()
    else:  
        editor_err = "\\n Text editor \'" + user_editor + "\' not found. Please select a different one.\n"
        if not programmExists(user_editor, editor_err):
            sys.exit()
        text_editor = user_editor
    return text_editor

def runShellCmd(shell_cmd_line):
    """Run a shell command line
    """
    # Prepare the environment variables
    spbench_path = os.path.abspath(os.path.dirname(os.path.realpath(__file__)) + "/../../")
    env_vars_file = spbench_path + "/libs/libraries_env_vars.json"

    # Read the environment variables from the JSON file
    with open(env_vars_file) as f:
        env_vars = json.load(f)
    # Combine them with the current environment variables
    lib_env_vars = os.environ.copy()
    lib_env_vars.update(env_vars)
    
    if(utils.python_3 == 3):
        try:
            retcode = subprocess.call(shell_cmd_line, shell=True, env=lib_env_vars)
            if -retcode < 0:
                print(" Process was terminated by signal", -retcode, file=sys.stderr)
                print("\n Unsuccessful execution\n")
            return retcode
        except OSError as e:
            print(" Execution failed:", e, file=sys.stderr)
            print()
            return e
        except KeyboardInterrupt as e:
            print(" KeyboardInterrupt")
            print("\n Unsuccessful execution\n")
            sys.exit()
    else:
        os.system(shell_cmd_line)

def runShellWithReturn(shell_cmd_line):
    """Run a shell command line and return the output from the command
    """
    # Prepare the environment variables
    spbench_path = os.path.abspath(os.path.dirname(os.path.realpath(__file__)) + "/../../")
    env_vars_file = spbench_path + "/libs/libraries_env_vars.json"

    # Read the environment variables from the JSON file
    with open(env_vars_file) as f:
        env_vars = json.load(f)
    # Combine them with the current environment variables
    lib_env_vars = os.environ.copy()
    lib_env_vars.update(env_vars)

    if(utils.python_3 == 3):
        #success = False
        try:
            output = subprocess.check_output(shell_cmd_line, stderr=subprocess.STDOUT, shell=True, env=lib_env_vars).decode()
            #success = True 
        except subprocess.CalledProcessError as e:
            output = e.output.decode() + "\n Unsuccessful execution\n"
            #print(output)
            #print("\n Unsuccessful execution\n")
        except Exception as e:
            # check_call can raise other exceptions, such as FileNotFoundError
            output = str(e) + "\n Unsuccessful execution\n"
            #print(output)
            #print("\n Unsuccessful execution\n")
        except KeyboardInterrupt as e:
            print(" KeyboardInterrupt")
            print("\n Unsuccessful execution\n")
            sys.exit()
        return(output)
    else:
        os.system(shell_cmd_line)

# compute and return the md5 value of a file
def md5(file_name):
    hash_md5 = hashlib.md5()
    with open(file_name, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()

# check if a file exists
def fileExists(file):
    return bool(os.path.exists(file))

# check if a directory exists
def dirExists(file):
    return bool(os.path.isdir(file))

# check if an output file exists
def outputExists(output_file):
    if os.path.exists(output_file): 
        return True
    print(" Expected output file not found:\n " + output_file)
    print("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
    return False
