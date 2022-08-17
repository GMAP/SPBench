## 
 ##############################################################################
 #  File  : configure_option.py
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

def edit_json_func(spbench_path, args):
    #TODO
    #diferenciar flags da PPI e as do compilador (problema com spar-ferret)
    
    if args.benchmark_id == 'all':
        print("\n Error!! You cannot configure all benchmarks at once.\n Try again using a single benchmark ID.\n")
        sys.exit()

    # get the selected benchmark (it is a list)
    selected_benchmark = registryDicToList(filterBenchRegByBench(getBenchRegistry(spbench_path), args.benchmark_id))

    app_id = selected_benchmark[0]["app_id"]
    ppi_id = selected_benchmark[0]["ppi_id"]
    bench_id = selected_benchmark[0]["bench_id"]

    # get the config.json path
    config_file = spbench_path + "/apps/" + app_id + "/" + ppi_id + "/" + bench_id + "/config.json"

    if not fileExists(config_file):
        print("\n Error!! JSON configuration file not found at:\n " + config_file + "\n")
        sys.exit()

    editor_err = "\\n Text editor \'" + args.user_editor + "\' not found. Please select a different one."
    editorChecking(args.user_editor, editor_err)

    # generate a command line to run
    cmd_line = args.user_editor + " " + config_file
    print("---------------------------------------------")
    print(" Running -> " + cmd_line)
    print("---------------------------------------------")
    # run the command line
    runShellCmd(cmd_line)

    #build a new makefile
    make_gen(spbench_path, selected_benchmark[0])
    
    sys.exit()
