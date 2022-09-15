## 
 ##############################################################################
 #  File  : reset_option.py
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

def reset_operators_func(spbench_path, args):
    """reset all operators from a benchmark to default
    """
    if args.benchmark_id == 'all':
        print("\n Warning!!! You are trying to reset all operators from all benchmarks.\n You will loose any previous modifications.\n")
        if not askToProceed():
            sys.exit()
    ##
    # get the selected benchmarks
    ##
    selected_benchmarks = registryDicToList(filterRegistry(getBenchRegistry(spbench_path), args))

    ##
    # Reset the operators of each benchmark on the list
    ##
    for benchmark in selected_benchmarks:
        
        app_id = benchmark["app_id"]
        ppi_id = benchmark["ppi_id"]
        bench_id = benchmark["bench_id"]
        nsources = benchmark["nsources"]

        ns_sufix=''
        if nsources:
            ns_sufix='_ns'

        if ppi_id == "spar":
            print("\n Warning!!! You are trying to reset the operators of a SPar benchmark.\n Benchmarks for SPar usually have custom operators.\n If you reset operators of a SPar benchmark that was originally provided by SPBench, it may not work anymore.\n")
            if not askToProceed():
                continue
        ##
        # get the path for the respective templates
        ##
        op_templates = (spbench_path + '/sys/apps/' + app_id + '/templates' + ns_sufix + '/')
  
        ##
        # get the path for the benchmark to be reset
        ##
        bench_path = spbench_path + "/benchmarks/" +  app_id + "/" + ppi_id + "/" + bench_id + ""

        ##
        # last set of operators before the reset are kept, for backup purposes
        # they are moved into a _old directory
        ##
        if dirExists (bench_path + '/operators'):
            if dirExists (bench_path + '/operators_old'):
                runShellCmd('rm -r ' + bench_path + '/operators_old')
            runShellCmd('mv ' + bench_path + '/operators ' + bench_path + '/operators_old')
        
        ##
        # header file is deleted
        ##
        if fileExists (bench_path + ' ' + app_id + '.hpp'):
            runShellCmd('rm ' + bench_path + ' ' + app_id + '.hpp')
        
        ##
        # new operators are copied from template
        ##
        runShellCmd('cp ' + op_templates + app_id +".hpp " + bench_path)
        runShellCmd('cp -r ' + op_templates + "operators " + bench_path)

        print("\n -> " + bench_id + " operators were reset!\n")
        print(" -> Old operators are available (until next reset) at: \n  " + bench_path +"/operators_old \n")

    sys.exit()


