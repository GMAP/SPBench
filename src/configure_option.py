## 
 ##############################################################################
 #  File  : configure_option.py
 #
 #  Title : SPBench-CLI Configure Option
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

from src.utils import *
from src.make_gen import *

def configure_func(spbench_path, args):
    
    if args.benchmark_id == 'all':
        print("\n Error!! You cannot configure all benchmarks at once.\n Try again using a single benchmark ID.\n")
        sys.exit()

    # get the selected benchmark (it is a list)
    selected_benchmark = registryDicToList(filterBenchRegByBench(getBenchRegistry(spbench_path), args.benchmark_id))

    app_id = selected_benchmark[0]["app_id"]
    ppi_id = selected_benchmark[0]["ppi_id"]
    bench_id = selected_benchmark[0]["bench_id"]

    # get the config.json path
    config_file = spbench_path + "/benchmarks/" + app_id + "/" + ppi_id + "/" + bench_id + "/config.json"

    if not fileExists(config_file):
        print("\n Error!! JSON configuration file not found at:\n " + config_file + "\n")
        sys.exit()


    # get a text editor if there is one
    text_editor = getTextEditor(args.user_editor)

    # generate a command line to run
    cmd_line = text_editor + " " + config_file
    print("---------------------------------------------")
    print(" Running -> " + cmd_line)
    print("---------------------------------------------")

    # compute md5 hash of the json file    
    json_md5 = md5(config_file)
    
    # run the command line
    runShellCmd(cmd_line)

    # if nothing was changed, exit
    if(json_md5 == md5(config_file)):
        sys.exit()

    #build a new makefile
    print(" Updating compiling configurations for: " + selected_benchmark[0]["bench_id"] + "\n")
    make_gen(spbench_path, selected_benchmark[0])
    
    sys.exit()
