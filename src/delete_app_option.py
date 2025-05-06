## 
 ##############################################################################
 #  File  : delete_app_option.py
 #
 #  Title : SPBench-CLI Delete Application Option
 #
 #  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 #
 #  Date  : Nov. 29, 2023
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
from .list_benchmarks_option import list_benchmarks_func
from .delete_input_option import del_input_func

# delete a app from the registry and sys/apps/ folder
def delete_app_func(spbench_path, args):

    # check if app_id is in apps list
    if args.app_id not in getAppsList(spbench_path):
        print("\n Application \'" + args.app_id + "\' not found in the registry!\n")
        print(" Please, check if the application ID is correct and try again.\n")
        print(" You can run \'./spbench list-apps\' to list all applications.\n")
        # ask if the user wants to force the delete anyway
        print(" Do you want to " + color.BOLD + "FORCE" + color.END + " the delete anyway?")
        print(" This will delete the /spbench/sys/apps/" + args.app_id + " folder, if any.\n")
        print(" Do you want to proceed?")
    else:
        print("\n This procedure will permanently remove the application from SPBench.\n")
        print(" Take in mind that it will not delete the associated input files.\n")
        print(" Please, double check the data to delete below before proceeding.\n")
        print(" ---------------------------------------------")
        print("    Application: " + color.BOLD + args.app_id + color.END)
        print(" ---------------------------------------------")

        # get app registry and print info
        app_registry = getAppsRegistry(spbench_path)[args.app_id]
        print("           Name: " + app_registry["name"])
        print("           Type: " + app_registry["type"])
        print(" Num. operators: " + str(app_registry["num_operators"]))
        print("    Description: " + app_registry["description"])
        print("          Notes: " + app_registry["notes"])
        print(" ---------------------------------------------")


    if not askToProceed():
        sys.exit()

    benchmarks_registry = getBenchRegistry(spbench_path)

    ##
    # Before deleting the application check if there is any
    # benchmarks associated with it in the benchmarks registry
    ##
    print("###########################################################################")
    if args.app_id in benchmarks_registry:
        print("\n The following benchmarks are associated with this application.\n")

        # get the benchmarks from app_id
        #benchmarks_list = filterBenchRegByApp(benchmarks_registry, args.app_id, True)

        # print the benchmarks of this app
        list_benchmarks_func(spbench_path, args, True)

        # ask to remove the benchmarks associated with the app in benchmarks/ folder
        print("\n Do you want to " + color.BOLD + "PERMANENTLY REMOVE" + color.END + " all of them from SPBench?")

        if not askToProceed():
            sys.exit()

        ##
        # delete the benchmarks from the SPBench file system
        ##
            
        # get the benchmarks registry
        #benchmarks_registry = getBenchRegistry(spbench_path)

        # delete the app id key from the benchmarks registry
        del benchmarks_registry[args.app_id]

        # save the benchmarks registry
        writeDicToBenchRegistry(spbench_path, benchmarks_registry)

        # delete the benchmarks from the SPBench file system
        runShellCmd("rm -rf " + spbench_path + "/benchmarks/" + args.app_id)

    ##
    # delete the app from the SPBench file system
    ##

    # delete app key from the app registry
    if args.app_id in getAppsList(spbench_path):
        deleteAppFromRegistry(spbench_path, args.app_id)

    # delete app folder from sys/apps/
    app_dir = spbench_path + "/sys/apps/" + args.app_id
    if dirExists(app_dir):
        runShellCmd("rm -rf " + app_dir)

    print("\n \'" + color.BOLD + args.app_id + color.END + "\' deleted from SPBench!\n")

    print("###########################################################################")

    # ask if the user wants to delete the inputs associated with this app from registry
    print("\n Do you also want to remove all inputs associated with this application?")

    # create a custom args object to call the delete input function
    class Custom_del_input_args:
        def __init__(self, app_id):
            self.app_id = app_id
            self.input_id = 'all'

    del_input_func(spbench_path, Custom_del_input_args(args.app_id), True)

    print("###########################################################################")

    sys.exit()
