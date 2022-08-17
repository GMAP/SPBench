## 
 ##############################################################################
 #  File  : compile_option.py
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
#from register_option import *

from sys import version_info 
python_3 = version_info[0]

def compile_func(spbench_path, args):

    # get the selected benchmarks
    selected_benchmarks = registryDicToList(filterRegistry(getBenchRegistry(spbench_path), args))
    
    # succeed flag for later verifications
    compilation_succeed = False

    for benchmark in selected_benchmarks:
        app_id = benchmark["app_id"]
        ppi_id = benchmark["ppi_id"]
        bench_id = benchmark["bench_id"]

        print("-------------------------------------------")
        print('-> Compiling ' + bench_id + '...')
        print("-------------------------------------------")

        # search for a programm name or run for them all
        bin_path = spbench_path + "/bin/" + app_id + "/" + ppi_id
        if not dirExists(bin_path):     #if there is not a /bin directory, create one
            os.makedirs(bin_path)

        #path where make will run
        programm_path = spbench_path + "/apps/" + app_id + "/" + ppi_id + "/" + bench_id + "/" 

        # Build the makefile if it does not exist yet
        if not fileExists(programm_path + 'Makefile'):
            make_gen(spbench_path, benchmark)

        # check if exists the make directory
        if not dirExists(programm_path):
            print("Error!!\n-> Directory [" + programm_path + "] not found.\n-> Check if it exists or try to register it again!")
            sys.exit()
        
        if args.clean:
            runShellCmd("make -C " + programm_path + " clean")

        #else:
        #    if app_id == 'ferret':
        #        # temporary (or not) solution to solve ferret compiling problem
        #        bin_file = spbench_path + '/bin/' + app_id + "/" + ppi_id + "/" + bench_id
        #        if fileExists(bin_file):
        #            runShellCmd("make -C " + programm_path + " clean")

        # Avoid using -j for ferret, because it is already used by its submake routine
        make_cmd = ""
        if app_id == "ferret":
            make_cmd = ("make -C " + programm_path)
        else:
            make_cmd = ("make -C " + programm_path + " -j$(nproc)")
        
        # compile the programm
        if(os.WEXITSTATUS(os.system(make_cmd))):
            print("\n ---------------------------------------")
            print(" Compilation failed!")
            print(" Version: " + bench_id)
            print("\n Common problems regarding SPBench:")
            print(" - Required library not installed.")
            print("   See libs/README.md and ppis/README.md \n   and make sure everything is setup.\n")
            print(" - Required library is installed but not loaded.")
            print("   Run \'source setup_vars.sh\'\n")
            print(" - Missing or wrong compiling parameters.")
            print("   You can use the command \'configure\' \n   to check and modify compiling dependencies.")
            print("   Run: ./spbench configure -benchmark " + bench_id)
            #else:
            #    print("\n---------------------------------------")
            #    print(" Compilation succeed!")
            #    print(" Version: " + bench_id)
            print(" ---------------------------------------\n")
            
        compilation_succeed = True

        # if it is compiling a single benchmark, ends here
        #if (args.benchmark_id.lower() != 'all'):
        #    sys.exit()
    print("")
    if not compilation_succeed:
        print("Compilation failed. \'" + args.benchmark_id + "\' not found!!\n")

    sys.exit()       
