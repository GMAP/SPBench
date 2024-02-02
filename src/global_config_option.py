## 
 ##############################################################################
 #  File  : global_config_option.py
 #
 #  Title : SPBench Global Compiling Configuration
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

from src.utils.shell import *
from src.utils.dict import *
from src.codeGenerators.make_gen import *

def global_config_func(spbench_path, args):

    # get the config.json path
    config_file = spbench_path + "/benchmarks/global_config.json"

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
    global_json_md5 = md5(config_file)
    
    # run the command line
    runShellCmd(cmd_line)

    # if nothing was changed, exit
    if(global_json_md5 == md5(config_file)):
        sys.exit()
    
    # Fake struct to get all benchmarks
    args.benchmark_id = "all"
    args.ppi_id = ""
    args.app_id = ""

    benchmarks_to_run = registryDicToList(filterRegistry(getBenchRegistry(spbench_path), args))

    for benchmark in benchmarks_to_run:
        print(" Updating compiling configurations for: " + benchmark["bench_id"])
        make_gen(spbench_path, benchmark)

    
    sys.exit()
