## 
 ##############################################################################
 #  File  : op_edit_option.py
 #
 #  Title : SPBench-CLI Operator Edition Option
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

from src.errors import *
from src.utils.shell import *
from src.utils.dict import *

def edit_op_source_func(spbench_path, args):

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

    operators_dict = getDictFromJSON(operators_list_file)

    if(operator_id not in operators_dict):
        print("\n Error, operator ID \'" + operator_id + "\' not found!\n")
        print(" The available operators for this benchmark are: ")
        for key in operators_dict:
            print("  - " + key)

        print("\n You can add a new operator using the command [new-op]")
        print(" Run \'./spbench new-op -h\' for more info.\n")
        
        sys.exit()

    operator_source = spbench_path + "/benchmarks/" + app_id + "/" + ppi_id + "/" + bench_id + "/operators/src/" + operator_id + "_op.cpp"

    if not fileExists(operator_source):
        print("\n Error, operator source file not found: \n " + operator_source + "\n")
        print("\n You can use the SPBench command [reset-op] to\n restore all operators to their original form.")
        print(" But CAUTION!!! It will remove any new operator or\n any modifications added to the original operators.\n")
        sys.exit()

    # get a text editor if there is one
    text_editor = getTextEditor(args.user_editor)

    # generate a command line to run
    cmd_line = text_editor + " " + operator_source
    print("---------------------------------------------")
    print(" Running -> " + cmd_line)
    print("---------------------------------------------")
    
    # run the command line
    runShellCmd(cmd_line)
    

    #if there is not a makefile yet, build one
    #make_path = spbench_path + "/benchmarks/" + app_id + "/" + ppi_id + "/" + bench_id + "/Makefile"
    #if not fileExists(make_path):
    #    make_gen(spbench_path, selected_benchmark[0])

    #sys.exit()
