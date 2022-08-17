## 
 ##############################################################################
 #  File  : clean_option.py
 #
 #  Title : SPBench commands manager
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
import os

from src.utils import *
from src.make_gen import *

def clean_func(spbench_path, args):
    
    # get the selected benchmarks
    selected_benchmarks = registryDicToList(filterRegistry(getBenchRegistry(spbench_path), args))
    
    for benchmark in selected_benchmarks:
        app_id = benchmark["app_id"]
        ppi_id = benchmark["ppi_id"]
        bench_id = benchmark["bench_id"]

        print("-------------------------------------------")
        print('-> Cleaning ' + bench_id + '...')
        print("-------------------------------------------")

        # search for a programm name or run for them all
        bin_path = spbench_path + "/bin/" + app_id + "/" + ppi_id
        if not dirExists(bin_path):     #if there is not a /bin directory, create one
            os.makedirs(bin_path)

        #path where make will run
        benchmark_path = spbench_path + "/apps/" + app_id + "/" + ppi_id + "/" + bench_id + "/" 

        # check if exists the make directory
        if not dirExists(benchmark_path):
            print("Error!!\n-> Benchmark not found: " + benchmark_path + "\n")
            sys.exit()

        # Build the makefile if it does not exist yet
        if not fileExists(benchmark_path + 'Makefile'):
            make_gen(spbench_path, benchmark)

        runShellCmd("make -C " + benchmark_path + " clean")

        # check and delete it if exists the directory that stores old operators
        old_operators = benchmark_path + "operators_old"
        if dirExists(old_operators):
            print("rm -rf " + old_operators)
            runShellCmd("rm -rf " + old_operators)

    sys.exit()       
