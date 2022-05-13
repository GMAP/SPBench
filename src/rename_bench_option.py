## 
 ##############################################################################
 #  File  : new_bench_option.py
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

#from delete_option import *

# rename a benchmark from the suite
def rename_bench_func(spbench_path, args):

    if args.old_bench_id == args.new_bench_id:
        print("\n Error!! The old and new IDs must be different.\n")
        sys.exit()

    # check if it is not a reserved word
    if (args.new_bench_id in reserved_words):
        print("\n " + args.new_bench_id + " is a SPBench reserved word")
        print(" You can not use it to name a benchmark.\n")
        sys.exit()

    # check if the old benchmark exists
    if not benchmarkExists(spbench_path, args.old_bench_id):
        print("\n Error!! The benchmark you selected to rename does not exist.")
        print("\n Please, make sure you wrote the correct id.\n")
        sys.exit()

    # check if the new benchmark exists
    if benchmarkExists(spbench_path, args.new_bench_id):
        print("\n Error!! The benchmark id \'" + args.new_bench_id + "\' already exists.")
        print("\n Please, insert a different new name for the benchmark.\n")
        sys.exit()

    old_bench_data = registryDicToList(filterBenchRegByBench(getBenchRegistry(spbench_path), args.old_bench_id))

    app_id = old_bench_data[0]["app_id"]
    ppi_id = old_bench_data[0]["ppi_id"]
    old_bench_id = old_bench_data[0]["bench_id"]
    new_bench_id = args.new_bench_id
        
    old_bench_path = spbench_path + "/apps/" +  app_id + '/' + ppi_id + '/' + old_bench_id + "/"
    new_bench_path = spbench_path + "/apps/" +  app_id + '/' + ppi_id + '/' + new_bench_id + "/"

    ok_to_rename = False
    if dirExists(old_bench_path):
        if fileExists(old_bench_path + old_bench_id + '.cpp'):
            if fileExists(old_bench_path + 'config.json'):
                ok_to_rename = True
            else:
                print(old_bench_path + 'config.json is missing!\n')
        else:
            print(old_bench_path + old_bench_id + '.cpp file is missing!\n')
    else:
        print("Files for benchmark " + old_bench_id + ' not found at ' + old_bench_path +  + '\n')


    # if it is all ok to create a new benchmark, do it
    if ok_to_rename:

        # rename in the registry
        registry_dic = getBenchRegistry(spbench_path)

        registry_dic[app_id][ppi_id][new_bench_id] = registry_dic[app_id][ppi_id].pop(old_bench_id)

        # rename files
        os.system('mv ' + old_bench_path + " " + new_bench_path)
        os.system('mv ' + new_bench_path + old_bench_id +".cpp " + new_bench_path + new_bench_id + ".cpp")

        # rewrite dictionary to JSON registry file
        writeDicToBenchRegistry(spbench_path, registry_dic)

        print('\n  ' + old_bench_id + " -> " + new_bench_id + '\n')

        sys.exit()
    
    print('\n Error while renaming your new benchmark\n')
    sys.exit()

    
