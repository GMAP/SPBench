## 
 ##############################################################################
 #  File  : list_benchmarks_option.py
 #
 #  Title : SPBench-CLI List Benchmarks Option
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

from sys import version_info

from src.utils.dict import *
from src.utils.utils import *

python_3 = version_info[0]

#print all registry
def list_benchmarks_func(spbench_path, args, continue_execution = False):

    # retrieve the benchmarks registry
    registry_dic = getBenchRegistry(spbench_path)
    
    print(" -------------------------------------------------------------------")
    print( color.BOLD +" APPLICATION           PPI              SOURCE     BENCHMARK NAME       "+color.END)
    print(" -------------------------------------------------------------------")

    bench_found = False

    #print all registered benchmarks for the selected application
    if(args.app_id):
        for ppi_key, value in registry_dic[args.app_id].items():
            for bench_key in registry_dic[args.app_id][ppi_key]:
                print(" " + args.app_id.ljust(20) + "| " + ppi_key.ljust(15) + "| " + registry_dic[args.app_id][ppi_key][bench_key].ljust(9) + "| " + color.BOLD + bench_key + color.END)
                bench_found = True
        if not bench_found:
            print(" " + args.app_id.ljust(20) + "| " + "none".ljust(15) + "| " + "none".ljust(9) + "| " + color.BOLD + "none" + color.END)

    #print all registered benchmarks for the selected application
    elif(args.ppi_id):
        for app_key in registry_dic:
            for ppi_key, value in registry_dic[app_key].items():
                if ppi_key == args.ppi_id:
                    for bench_key in registry_dic[app_key][args.ppi_id]:
                        print(" " + app_key.ljust(20) + "| " + args.ppi_id.ljust(15) + "| " + registry_dic[app_key][args.ppi_id][bench_key].ljust(9) + "| " + color.BOLD + bench_key + color.END)
                        bench_found = True
        if not bench_found:
            print(" " + "none".ljust(20) + "| " + args.ppi_id.ljust(15) + "| " + "none".ljust(9) + "| " + color.BOLD + "none" + color.END)


    else: # print all registry
        for app_key in registry_dic:
            for ppi_key, value in registry_dic[app_key].items():
                for bench_key, value in registry_dic[app_key][ppi_key].items():
                    print(" " + app_key.ljust(20) + "| " + ppi_key.ljust(15) + "| " + registry_dic[app_key][ppi_key][bench_key].ljust(9) + "| " + color.BOLD + bench_key + color.END)
    
    print(" -------------------------------------------------------------------")

    if not continue_execution:
        sys.exit()