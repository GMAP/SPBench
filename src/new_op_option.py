## 
 ##############################################################################
 #  File  : new_op_option.py
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

def new_op_func(spbench_path, args):
    """!@brief Function for the new operator command.

    This function is used to add a new operator to a given benchmark in the SPBench suite.

    @param spbench_path Path of the SPBench root.
    @param args Arguments captured in the argparse methods.
    """

    if args.benchmark_id == 'all':
        print("\n Error!! You cannot edit all benchmarks at once.\n Try again using a single benchmark ID.\n")
        sys.exit()

    operator_id = args.operator_id

    # get the selected benchmark (it is a list)
    selected_benchmark = registryDicToList(filterBenchRegByBench(getBenchRegistry(spbench_path), args.benchmark_id))

    app_id = selected_benchmark[0]["app_id"]
    ppi_id = selected_benchmark[0]["ppi_id"]
    bench_id = selected_benchmark[0]["bench_id"]

    operators_list_file = spbench_path + "/benchmarks/" + app_id + "/" + ppi_id + "/" + bench_id + "/operators/operators.json"
    
    if not fileExists(operators_list_file):
        print("\n Error, file not found (operators list file): \n " + operators_list_file + "\n")
        sys.exit()

    operators_dict = getRegistry(operators_list_file)

    if(operator_id.lower() in operators_dict):
        print("\n Error, operator ID \'" + operator_id + "\' already exists!\n")
        print(" These are the registered operators for this benchmark: ")
        print("  - source")
        for key in operators_dict:
            print("  - " + key)
        print("  - sink")
        print("\n You can delete an operator using the command [delete-op]")
        print(" Run \'./spbench delete-op -h\' for more info.\n")
        sys.exit()

    # check if it is not a reserved word
    if(operator_id.lower() in reserved_words):
        print("\n Error, \'" + operator_id + "\' is a SPBench reserved word!\n")
        print(" Please, select a different name for your operator. \n")
        sys.exit()

    if operator_id == args.copy_from:
        print("\n Error!! You cannot name your new operator using the same name of the operator you are copying from.\n")
        sys.exit()

    ######################
    #
    #  This code is not finished, thus not working
    #  It is not available for users
    #
    ######################

    sys.exit()
