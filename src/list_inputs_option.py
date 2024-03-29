## 
 ##############################################################################
 #  File  : list_inputs_option.py
 #
 #  Title : SPBench-CLI List Inputs Option
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

from src.utils.utils import *
from src.utils.dict import *

#print all inputs
def list_inputs_func(spbench_path, args):

    inputs_registry = getInputsRegistry(spbench_path)

    # print all inputs for the selected application
    if(args.app_id):
        print("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
        print("-> " + args.app_id)
        print("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
        app_inputs = inputs_registry[args.app_id]
        for key in app_inputs:
            print(color.BOLD+"            ID: " + key + color.END)
            if bool(inputs_registry[args.app_id][key].get('md5_test')):
                print("  Expected md5: " + app_inputs[key]["md5_test"])
            print("  Input string: " + '\"' + app_inputs[key]["input"].replace('$SPB_HOME', spbench_path) + '\"')
            print("")

    else: 
        # print all inputs for all applications

        # get a list of the main keys (applications)
        apps_inputs_list = list(inputs_registry.keys())

        for app in apps_inputs_list:
            print("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
            # check if the app is in the apps registry
            if not app in getAppsList(spbench_path):
                print("-> " + app + (color.RED + " (Application not included in the database)" + color.END))
            else:
                print("-> " + app)
            print("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
            app_inputs = inputs_registry[app]
            for key in app_inputs:
                print(color.BOLD+"            ID: " + key + color.END)
                if bool(inputs_registry[app][key].get('md5_test')):
                    print("  Expected md5: " + app_inputs[key]["md5_test"])
                print("  Input string: " + '\"' + app_inputs[key]["input"].replace('$SPB_HOME', spbench_path) + '\"')
                print("")
    print("")
    sys.exit()
