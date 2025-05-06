## 
 ##############################################################################
 #  File  : delete_input_option.py
 #
 #  Title : SPBench-CLI Delete Input Option
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

from src.utils.dict import *
from src.utils.usage import *

# delete a input from the registry
def del_input_func(spbench_path, args, continue_execution = False):

    inputs_registry = getInputsRegistry(spbench_path)

    # Check if the chosen app exists
    if args.app_id not in inputs_registry:
        print("\n There is no entry for application \'" + args.app_id + "\' in the inputs registry!\n")
        return None if continue_execution else sys.exit()

    # check if this input is already registered for this application
    if args.input_id != 'all' and args.input_id not in inputs_registry[args.app_id]:
        print("\n Input ID not found for " + args.app_id + "\n")
        return None if continue_execution else sys.exit()

    print("\n This procedure will remove the input only from the registry.\n")
    print(" It will not delete the associated input files from the file system.\n")
    print(" Please, double check the data to delete below before proceeding.\n")
    print(" Application: " + args.app_id)
    print("    Input ID: " + (args.input_id if args.input_id != 'all' else "all the inputs of this application"))
    if args.input_id != 'all':
        print("       Input: " + inputs_registry[args.app_id][args.input_id]["input"])
        if inputs_registry[args.app_id][args.input_id]["md5_test"]:
            print(" Testing md5: " + inputs_registry[args.app_id][args.input_id]["md5_test"])
        else:
            print(" Testing md5: none")

    if not askToProceed():
        return None if continue_execution else sys.exit()

    # delete all inputs for the selected application
    if args.input_id == 'all':
        del inputs_registry[args.app_id]
        print("\n All inputs for " + args.app_id + " were deleted from the registry!\n")

    else:
        # delete input key from the dictionary
        del inputs_registry[args.app_id][args.input_id]

        # check if the application still has inputs
        if not bool(inputs_registry[args.app_id]):
            # delete the application key from the dictionary
            del inputs_registry[args.app_id]

        print("\n " + args.input_id + " was deleted from the registry!\n")

    # write the dictionary to the json registry file      
    writeDicToInputRegistry(spbench_path, inputs_registry)

    if not continue_execution: sys.exit()
