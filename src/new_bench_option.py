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

from src.delete_option import delete_benchmark

from sys import version_info 
python_3 = version_info[0]

#add a new benchmark to the suite
def new_func(spbench_path, args):

    # check if it is not a reserved word
    if args.benchmark_id in reserved_words:
        print("\n " + args.benchmark_id + " is a SPBench reserved word")
        print(" You can not use it to name a benchmark.\n")
        sys.exit()

    # Check if the chosen app exists
    if args.app_id not in apps_list:
        print("\n Application \'" + args.app_id + "\' not found!\n")
        sys.exit()

    if args.benchmark_id == args.copy_from:
        print("\n Error!! You cannot name your benchmark using the same copy-from ID\n")
        sys.exit()
    
    # set the path of the copy-from benchmark
    ok_to_copy = False
    copy_from_path = ''
    copy_from = ''
    nsource = ''

    if not args.copy_from: # if no copy-from benchmark was selected, set the copy-from path to the sequential template
        if args.nsources:
            nsource = '_ns'
        
        copy_from_path = (spbench_path + '/sys/apps/' + args.app_id + '/templates' + nsource + '/')

        #if args.nsources:
        #    copy_from = args.app_id + '_ns'
        #else:
        copy_from = args.app_id
        
        ok_to_copy = True
    else: # search for the selected benchmark to make a new copy

        if not benchmarkExists(spbench_path, args.copy_from):
            print("\n Error!! The benchmark you selected to copy from does not exists.")
            print("\n You can run \'./spbench list\' to see all available benchmarks to copy from.\n")
            sys.exit()

        copy_from_data = registryDicToList(filterRegistry(getBenchRegistry(spbench_path), args, True))

        copy_from_app_id = copy_from_data[0]["app_id"]
        copy_from_ppi_id = copy_from_data[0]["ppi_id"]
        copy_from_bench_id = copy_from_data[0]["bench_id"]
        
        #check if the copy-from is not from the same app
        if copy_from_app_id != args.app_id: 
            print("\n "+color.BOLD+"WARNING"+color.END+": You are trying to make a new copy from a different application.\n")
            print("  Selected application: " + args.app_id)
            print(" Copy-from application: " + copy_from_app_id)
            if not askToProceed():
                sys.exit()

        # check if the type of source different from the copy-from benchmark
        if args.nsources and not copy_from_data[0]["nsources"]:
            print(" ---------------------------------------------------------------")
            print(color.BOLD + "                            >> WARNING <<                         " + color.END)
            print(" ---------------------------------------------------------------")
            print("\n You are trying to create a new n-sources benchmark, but the\n benchmark you selected to copy-from is a single source one.\n")
            print("  Selected benchmark: " + args.benchmark_id)
            print(" Copy-from benchmark: " + copy_from_bench_id)
            print(color.BOLD+"\n If you proceed, your benchmark will be registered as single source."+color.END)
            if not askToProceed():
                sys.exit()
            args.nsources = False
            
        # check if the type of source is different from the copy-from benchmark
        if not args.nsources and copy_from_data[0]["nsources"]:
            print(" ---------------------------------------------------------------")
            print(color.BOLD + "                            >> WARNING <<                         " + color.END)
            print(" ---------------------------------------------------------------")
            print("\n You are trying to create a new single source benchmark, but the\n benchmark you selected to copy-from is a n-sources one.\n")
            print("  Selected benchmark: " + args.benchmark_id)
            print(" Copy-from benchmark: " + copy_from_bench_id)
            print(color.BOLD + "\n If you proceed, your benchmark will be registered as n-sources." + color.END)
            if not askToProceed():
                sys.exit()
            args.nsources = True
            
        copy_from_path = spbench_path + "/apps/" +  copy_from_app_id + '/' + copy_from_ppi_id + '/' + copy_from_bench_id + "/"
        if fileExists(copy_from_path + copy_from_bench_id + '.cpp'):
            if fileExists(copy_from_path + 'config.json'):
                copy_from = copy_from_bench_id
                ok_to_copy = True
            else:
                print(copy_from_path + 'config.json is missing!\n')
        else:
            print(copy_from_path + copy_from_bench_id + '.cpp file is missing!\n')


    registry_dic = getBenchRegistry(spbench_path)
    
    if benchmarkExists(spbench_path, args.benchmark_id):

        existent_data = registryDicToList(filterRegistry(getBenchRegistry(spbench_path), args))

        existent_app_id = existent_data[0]["app_id"]
        existent_ppi_id = existent_data[0]["ppi_id"]
        existent_bench_id = existent_data[0]["bench_id"]

        #if (benchmark_id == args.benchmark_id): # Tell the user if benchmark id already exists
        print(" ---------------------------------------------------------------")
        print(color.BOLD + "                            >> WARNING <<                         " + color.END)
        print(" ---------------------------------------------------------------")
        print("\n This benchmark name already exists!\n")
        print(" -> Existent benchmark data")
        print("   Benchmark: " + existent_bench_id)
        print(" Application: " + existent_app_id)
        print("         PPI: " + existent_ppi_id)
        
        print("\n The existent benchmark will be "+color.BOLD+"deleted"+color.END+" and replaced by:\n")
        print("   Benchmark: " + args.benchmark_id)
        print(" Application: " + args.app_id)
        print("         PPI: " + args.ppi_id)

        if existent_app_id != args.app_id: 
            print("\n "+color.BOLD+"CAUTION"+color.END+":\n This benchmark ID was previously assigned to a different\n application: " + args.app_id)
        if existent_ppi_id != args.ppi_id:
            print("\n "+color.BOLD+"CAUTION"+color.END+":\n This benchmark ID was previously assigned to a different\n PPI: " + args.ppi_id)

        if not askToProceed(): # delete the old benchmark
            sys.exit()    
        delete_benchmark(spbench_path, args)
        

    # set the path for the new benchmark
    new_path = spbench_path + "/apps/" +  args.app_id + "/" + args.ppi_id + "/" + args.benchmark_id + "/"

    if fileExists(new_path):
        print("\n WARNING: There is already a directory for this benchmark.")
        print(" -> " + new_path)
        print("\n" +color.BOLD+" >> Existent files in that directory will be deleted << " + color.END)
        if not askToProceed():
            sys.exit()
        os.system('rm -r ' + new_path)

    # if it is all ok to create a new benchmark, do it
    if ok_to_copy:
        os.makedirs(new_path) # copy the files from somewhere else (template or existent benchmark)
        
        os.system('cp ' + copy_from_path + copy_from +".cpp " + new_path + args.benchmark_id + ".cpp")

        os.system('cp ' + copy_from_path + args.app_id +".hpp " + new_path)
        
        os.system('cp ' + copy_from_path + "config.json " + new_path + "config.json")

        os.system('cp -r ' + copy_from_path + "operators " + new_path)

    else:
        print('\n Error while creating your new benchmark\n')
        sys.exit()

    if args.ppi_id not in registry_dic[args.app_id]:
        registry_dic[args.app_id].update({args.ppi_id:{}}) #update the dictionary with the new ppi

    # Add new benchmark to the app->ppi->list
    if args.nsources:
        registry_dic[args.app_id][args.ppi_id].update({args.benchmark_id:"multiple"})
    else:
        registry_dic[args.app_id][args.ppi_id].update({args.benchmark_id:"single"})

    # rewrite dictionary to JSON registry file
    writeDicToBenchRegistry(spbench_path, registry_dic)
    
    print("\n New benchmark called \'" + args.benchmark_id + "\' successfully added!\n")
    if args.nsources:
        print("\n You enabled multiple source support for this benchmark.\n")
    print(" ---------------------------------------------------------------")
    print(color.BOLD + "                         >> ATTENTION <<                         " + color.END)
    print(" ---------------------------------------------------------------")
    print(" To add your modifications into the new benchmark, run:\n")
    print(" "+color.BOLD+"./spbench edit -benchmark " + args.benchmark_id + " -editor [text_editor]\n\n"+color.END)
    print(" You must also configure it to add any required dependencies:\n")
    print(" "+color.BOLD+"./spbench configure -benchmark " + args.benchmark_id + " -editor [text_editor]"+color.END)
    print(" ---------------------------------------------------------------")

    sys.exit()
