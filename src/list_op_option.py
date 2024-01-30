## 
 ##############################################################################
 #  File  : list_op_option.py
 #
 #  Title : SPBench-CLI List Operators Option
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

from src.utils.shell import *
from src.utils.dict import *

#print all operators
def list_op_func(spbench_path, args):

    if args.benchmark_id == 'all':
        print("\n Error!! You cannot list the operators of all benchmarks at once.\n Try again using a single benchmark ID.\n")
        sys.exit()
    
    # get the selected benchmark (it is a list)
    selected_benchmark = registryDicToList(filterBenchRegByBench(getBenchRegistry(spbench_path), args.benchmark_id))

    app_id = selected_benchmark[0]["app_id"]
    ppi_id = selected_benchmark[0]["ppi_id"]
    bench_id = selected_benchmark[0]["bench_id"]

    operators_list_file = spbench_path + "/benchmarks/" + app_id + "/" + ppi_id + "/" + bench_id + "/operators/operators.json"
    
    if not fileExists(operators_list_file):
        print("\n Error, file not found (operators list file): \n " + operators_list_file)
        print("\n You can run [ ./spbench reset-op -bench " + bench_id + " ]\n to restore the operators to their original form.")
        print("\n Run [ ./spbench reset-op --help ] for more info\n")
        sys.exit()

    operators_dict = getRegistry(operators_list_file)

    print("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
    print("-> Available operators for " + bench_id)
    print("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")

    for key in operators_dict:
        print("  - " + key)
    print("")

    sys.exit()
