## 
 ##############################################################################
 #  File  : new_app.py
 #
 #  Title : SPBench-CLI New Operator Option
 #
 #  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 #
 #  Date  : August, 2022
 #
 #  Copyright (C) 2023 Adriano M. Garcia
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
import os

from src.utils.usage import *
from src.utils.dict import *
from src.utils.shell import *
from src.codeGenerators.new_app.write_operators import *
from src.codeGenerators.new_app.write_app_utils import *
from src.codeGenerators.new_app.write_app import *
from src.utils.dict import *
from .new_input_option import new_input_func
from .new_bench_option import new_bench_func

def new_app_func(spbench_path, args):
    """!@brief Function for the new app command.

    This function is used to add a new application to the SPBench suite.

    @param spbench_path Path of the SPBench root.
    @param args Arguments captured in the argparse methods.
    """

    # dictionary format of the applications
    # app_name: {
    #     "name": "Application name",
    #     "type": "User-defined application",
    #     "num_operators": N,
    #     "description": "This is a user-defined application.",
    #     "notes": "The original code of this application is a toy example. Edit it to create your own application."
    # }

    # check if the app_id is not in the apps registry
    if args.app_id in getAppsList(spbench_path):
        print("\n Error!! Application \'" + args.app_id + "\' already exists.\n")
        sys.exit()

    if args.app_id in reserved_words:
        print("\n Error!! \'" + args.app_id + "\' is a SPBench reserved word. Please, choose a different name for this application\n")
        sys.exit()

    # check if there is not space in the app_id
    if " " in args.app_id:
        print("\n Error!! Application ID cannot contain spaces.\n")
        sys.exit()

    # check if app_name is less than 50 characters
    if len(args.app_name) > 50:
        print("\n Error!! Application name must be less than 50 characters.\n")
        sys.exit()

    # check if args.operators list has at least one operator
    if len(args.operators_list) == 0:
        print("\n Error!! You must specify at least one operator for the new application.\n")
        sys.exit()

    # check if the operators in the list are not reserved words
    for operator in args.operators_list:
        if operator in reserved_words:
            print("\n Error!! \'" + operator + "\' is a SPBench reserved word. Please, choose a different name for this operator\n")
            sys.exit()

    # check if there is not two operators with the same name
    if len(args.operators_list) != len(set(args.operators_list)):
        print("\n Error!! There are two or more operators with the same name.\n")
        sys.exit()

    print("\n Warning!") 
    print(" - SPBench does not provide mechanisms to change this")
    print("   configuration after the application is created.")
    print("   If you want to change the name of the application") 
    print("   or operators, or operators order, you will need")         
    print("   to manually do it or create a new application.\n")

    # ask user to confirm the if the information about the new app and its operators is correct
    print("\n Please, confirm the information about the new application and if the operators are in the correct order: \n")
    print("  - Application ID  -> " + args.app_id)
    print("  - Operators: ")
    print("    1  -> Source")
    for operator in args.operators_list:
        print("    " + str((args.operators_list.index(operator) + 2)) + "  -> " + operator)
    print("    " + str((len(args.operators_list) + 2)) + "  -> Sink")

    if not askToProceed():
        sys.exit()

    new_app_id = args.app_id
    new_app_name = args.app_name if args.app_name else args.app_id
    operators_list = args.operators_list
    new_appdescription = args.description
    new_app_notes = args.notes

    # *******************************************************************************
    # create the new application directories structure inside the sys/apps folder
    # *******************************************************************************

    # File structure of a simple application:
    # SPBench
    #   |__ sys
    #       |__ apps
    #           |__ new_app
    #               |__ templates
    #               |   |__ operators
    #               |   |  |__ include
    #               |   |  |   |__ operator1_op.hpp
    #               |   |  |   |__ operatorN_op.hpp
    #               |   |  |__ src
    #               |   |  |   |__ operator1_op.cpp
    #               |   |  |   |__ operatorN_op.cpp
    #               |   |  |__ operators.json
    #               |   |__ new_app.hpp
    #               |   |__ new_app.cpp
    #               |   |__ config.json
    #               |__ new_app_utils.hpp
    #               |__ new_app_utils.cpp

    # set the path of the new application folder
    app_path = spbench_path + "/sys/apps/" + new_app_id
    # check if the new application folder already exists
    if os.path.exists(app_path):
        print("###########################################################################")

        print("\n Error!! Application \'" + new_app_id + "\' already exists in " + app_path + "\n")
        print(" Are you sure you want to overwrite it? This action cannot be undone using this interface.")
        if not askToProceed():
            sys.exit()
        # remove de existing application folder
        runShellCmd("rm -r " + app_path)

    # create the new application folder
    os.mkdir(app_path)

    # create the templates folder
    app_templates_path = app_path + "/templates"
    os.mkdir(app_templates_path)

    # create the operators template folder
    app_operators_path = app_templates_path + "/operators"
    os.mkdir(app_operators_path)

    # create the operators include folder
    app_operators_include_path = app_operators_path + "/include"
    os.mkdir(app_operators_include_path)

    # create the operators source folder
    app_operators_source_path = app_operators_path + "/src"
    os.mkdir(app_operators_source_path)

    # *******************************************************************************
    # create the operators
    # *******************************************************************************

    writeOperators(spbench_path, new_app_id, operators_list)

    # *******************************************************************************
    # create the user-side code of the application
    # *******************************************************************************

    writeApp(spbench_path, new_app_id, operators_list)

    # *******************************************************************************
    # create the new application utils file
    # *******************************************************************************

    writeAppUtils(spbench_path, new_app_id, operators_list)

    # *******************************************************************************
    # add the new app to the apps list (sys/apps/apps_registry.json)
    # *******************************************************************************

    # get the apps registry
    apps_registry_file = spbench_path + "/sys/apps/apps_registry.json"
    apps_registry = getDictFromJSON(apps_registry_file)

    # add the new app key to the apps registry
    apps_registry.update({new_app_id: {}})

    # add the information about the new app to the apps registry
    apps_registry[new_app_id] = {
        "name": new_app_name,
        "type": "User-defined application",
        "num_operators": len(operators_list),
        "description": new_appdescription,
        "notes": new_app_notes
    }

    # write the new apps registry
    writeDicTo(apps_registry_file, apps_registry)

    # *******************************************************************************
    print("###########################################################################")

    print("\n -> " + new_app_id + " created!\n")
    print(" Note that this application is only a " + color.BOLD + "TOY EXAMPLE." + color.END)
    print(" It does no relevant computation and " + color.BOLD + "IS NOT DESIGNED TO BE USED FOR BENCHMARKING" + color.END + ".")
    print(" Now you can use this application example as basis to build your own application.\n")
    print(" You can find the source code of it in:\n " + app_path + "\n")
    
    print(" You can create a new benchmark from this application using the following command:\n")
    print(" ./spbench new -bench <your_benchmark_name> -app " + new_app_id + "\n")

    print(" Run: './spbench new -h' for more details.\n")

    print(" More information about how to create a new application are included in the SPBench documentation.")
    print(" You can find the online documentation in " + color.BOLD + "https://spbench-doc.rtfd.io/" + color.END + " or a PDF version of it in:\n " + spbench_path + "/doc/\n")

    print("###########################################################################")

    # ask user if he wants to create a test input for the new application
    print("\n SPBench can create a test input file for this application for you.")

    if askToProceed():

        # create a directory for the new application inside the inputs folder if this directory does not exist
        inputs_path = spbench_path + "/inputs/" + new_app_id
        if not dirExists(inputs_path):
            os.mkdir(inputs_path)

        # create a file inside the inputs_path folder with a file containing 20 random numbers, one in each libne
        inputs_file_path = inputs_path + "/input.txt"
        if not fileExists(inputs_file_path):
            runShellCmd("seq 20 | shuf > " + inputs_file_path)

        # create a data structure to simulate the args object for the new_input_func() function
        class Custom_input_args:
            def __init__(self):
                self.app_id = new_app_id
                self.input_id = "test"
                self.input = inputs_file_path + " 20"
                self.md5_hash = ""

        # register the test input for the new application using the new_input_func() function
        Custom_input = Custom_input_args()
        new_input_func(spbench_path, Custom_input, True)

        print("\n This \'test\' input was created for this application so you can use it for testing purpose.\n")

    print("###########################################################################")
    
    # ask user if he wants to create a new benchmark from the new application
    print("\n SPBench can create a test benchmark based on this application for you.")

    if not askToProceed():
        sys.exit()

    # create a data structure to simulate the args object for the new_bench_func() function
    class Custom_bench_args:
        def __init__(self):
            self.benchmark_id = "test"
            self.app_id = new_app_id
            self.ppi_id = "sequential"
            self.copy_from = ""
            self.nsources = ""

    # register the benchmark for the new application using the new_bench_func() function
    Custom_bench = Custom_bench_args()
    new_bench_func(spbench_path, Custom_bench, True)

    print("\n This \'test\' benchmark was created for this application so you can use it for testing purpose.\n")

    # sugestion for compiling and executing the new benchmark
    print(" You can compile and run the new benchmark using the following commands:\n")
    print("   ./spbench compile -bench test")
    print("   ./spbench exec -bench test -input test\n")

    print("###########################################################################")
            
    sys.exit()

    #print("\n Usually, you will need to change the following files:")
    #for operator in operators_list:
    #    print(" - " + app_path + "/templates/operators/src/" + operator + "_op.cpp")
    #print(" In these files you will need to implement the operators logic.\n")
    #print(" Then, you will need to change the following files:")
    #print(" - " + app_path + "/" + new_app_id + "_utils.cpp")
    #print(" In this file you will need to implement the logic for the source and sink computations.\n")
    #print(" You also need to adapt the input_parser() function for your application's case.\n")
    #print(" In addition, you must include the specific initializations routines in init_bench() function.\n")
    #print(" Here you can also add global variables and other initializations routines.\n")
    #print(" You can also add code to the end_bench() function if needed.\n")
    #print(" Besides that, you will need to change the following files:")
    #print(" - " + app_path + "/" + new_app_id + "_utils.hpp")
    #print(" In this file you will need to adapt the workload_data and item_data structures to make sense with your application.\n")
    #print(" You can also declare here external global variables.\n")

    #print(" Finally, you will need to change the config.json file to set the parameters of your application.\n")
    #print(" You can find this file in " + app_path + "/templates/config.json\n")

    #print(" To use the new application, you first need to create a new benchamrk from it:\n")
    #print(" ./spbench new -bench <your_benchmark_name> -app " + new_app_id + " -ppi none\n")
    #print(" Then, you can compile and run the new benchmark:\n")
