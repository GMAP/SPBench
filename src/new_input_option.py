## 
 ##############################################################################
 #  File  : new_input_option.py
 #
 #  Title : SPBench-CLI New Input Option
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

import sys

from src.utils.usage import *
from src.utils.dict import *

#add a new input to the suite
def new_input_func(spbench_path, args):

    # check if it is not a reserved word
    if args.input_id in reserved_words:
        print("\n " + args.input_id + " is a SPBench reserved word")
        print(" You can not use it to name a benchmark.\n")
        sys.exit()

    inputs_registry = getInputsRegistry(spbench_path)

    # Check if the chosen app exists
    if args.app_id not in apps_list:
        print("\n Application \'" + args.app_id + "\' not found!\n")
        sys.exit()

    # If the app is not registered yet, create a new entry
    if args.app_id not in inputs_registry:
        inputs_registry.update({args.app_id:{}})

    # check if this input is already registered for this application
    if args.input_id in inputs_registry[args.app_id]:
        print("\n There is already an input named \'"+ args.input_id +"\' for " + args.app_id + ".\n")
        print(" Registered input -> " + inputs_registry[args.app_id][args.input_id]["input"])

        if not askToProceed():
            sys.exit()

    # update the dictionary with the new entry
    inputs_registry[args.app_id].update({args.input_id:{"input":args.input,"md5_test":args.md5_hash}})

    print("\n      New input added!\n")
    print("  Application: " + args.app_id)
    print("     Input ID: " + args.input_id)
    print(" Input string: " + '\"'  + args.input.replace('$SPB_HOME', spbench_path) + '\"')
    if args.md5_hash:
        print("  Testing md5: " + args.md5_hash + "\n")
    else:
        print("  Testing md5: no md5 key was added (Output correctness test will not be available for this input).\n")
        
    # write the dictionary to the json registry file
    writeDicToInputRegistry(spbench_path, inputs_registry)

    sys.exit()
