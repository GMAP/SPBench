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
import os

from src.utils import *

from src.delete_option import delete_benchmark

from sys import version_info 
python_3 = version_info[0]

def new_app_func(spbench_path, args):
    """!@brief Function for the new app command.

    This function is used to add a new application to the SPBench suite.

    @param spbench_path Path of the SPBench root.
    @param args Arguments captured in the argparse methods.
    """

    if appExists(spbench_path, args.app_id):
        print("\n Error!! Application \'" + args.app_id + "\' already exists.\n")
        sys.exit()

    if args.app_id in reserved_words:
        print("\n Error!! You cannot edit all benchmarks at once.\n Try again using a single benchmark ID.\n")
        sys.exit()

    # check if there is not space in the app_id
    if " " in args.app_id:
        print("\n Error!! Application ID cannot contain spaces.\n")
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

    # ask user to confirm the if the information about the new app and its operators is correct
    print("\n Please, confirm the information about the new application and if the operators are in the correct order: \n")
    print("  - Application ID  -> " + args.app_id)
    print("  - Operators: ")
    print("    1  -> Source")
    for operator in args.operators_list:
        print("    " + str((args.operators_list.index(operator) + 2)) + "  -> " + operator)
    print("    " + str((len(args.operators_list) + 2)) + "  -> Sink")
    print("\n")

    print(" Warning: SPBench does not provide mechanisms to change this")
    print("          configuration after the application is created.")
    print("          If you want to change the name of the application") 
    print("          or operators, or operators order, you will need")         
    print("          to manually do it or create a new application.\n")

    if not askToProceed():
        sys.exit()

    # create the new application folder
    app_path = spbench_path + "/sys/apps/" + args.app_id
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
    app_operators_source_path = app_operators_path + "/source"
    os.mkdir(app_operators_source_path)

    # create a json file inside the operators folder with the operators list as keys
    operators_list_file = app_operators_path + "/operators.json"
    operators_list = {}
    for operator in args.operators_list:
        operators_list.update({operator:{operator:""}})
    writeDicTo(operators_list_file, operators_list)

    

    # get the selected benchmark (it is a list)
 #   selected_benchmark = registryDicToList(filterBenchRegByBench(getBenchRegistry(spbench_path), args.benchmark_id))

#    app_id = selected_benchmark[0]["app_id"]
#    ppi_id = selected_benchmark[0]["ppi_id"]
#    bench_id = selected_benchmark[0]["bench_id"]

#    operators_list_file = spbench_path + "/benchmarks/" + app_id + "/" + ppi_id + "/" + bench_id + "/operators/operators.json"
    
#    if not fileExists(operators_list_file):
#        print("\n Error, file not found (operators list file): \n " + operators_list_file + "\n")
#        sys.exit()

#    operators_dict = getRegistry(operators_list_file)

    sys.exit()
