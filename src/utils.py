## 
 ##############################################################################
 #  File  : utils.py
 #
 #  Title : SPBench makefile generator
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

import hashlib
import sys
import os
import json

from sys import version_info 
python_3 = version_info[0]

class color:
    PURPLE = '\033[95m'
    CYAN = '\033[96m'
    DARKCYAN = '\033[36m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    END = '\033[0m'

# list of SPBench reserved words
reserved_words = {'all', 'bzip2', 'ferret', 'lane_detection', 'person_recognition'}

# list of supported apps
apps_list = ['bzip2', 'ferret', 'lane_detection', 'person_recognition']
apps_list_all = list(apps_list)
apps_list_all.insert(0, 'all')

def getBenchRegistry(spbench_path):
    """return a dictionay with the benchmarks registered data
    """
    registry_file = spbench_path + "/sys/registry.json"
    #check if the registry exists
    if os.path.exists(registry_file) == False:
        print("\n There is no benchmarks registered.\n Registry file not found at " + registry_file + "\n")
        sys.exit()
    return getRegistry(registry_file)

def getInputsRegistry(spbench_path):
    """return a dictionay with the inputs registered data
    """
    registry_file = spbench_path + "/sys/inputs_registry.json"
    #check if the registry exists
    if os.path.exists(registry_file) == False:
        print("\n There is no inputs registered.\n Registry file not found at " + registry_file + "\n")
        sys.exit()
    return getRegistry(registry_file)

def getRegistry(registry_file):
    """read the content of the json registry and store into a dictionary
    """
    registry = open(registry_file, 'r')
    registry_dic = {}
    with registry as f:
        registry_dic = json.load(f)
    registry.close()
    return registry_dic

# compute and return the md5 value of a file
def md5(file_name):
    hash_md5 = hashlib.md5()
    with open(file_name, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()

# check if a file exists
def fileExists(file):
    return bool(os.path.exists(file))

# check if a directory exists
def dirExists(file):
    return bool(os.path.isdir(file))

# check if an output file exists
def outputExists(output_file):
    if os.path.exists(output_file): 
        return True
    print(" Expected output file not found:\n " + output_file)
    print("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
    return False

# check if string is blank
def isNotBlank (my_string):
    return bool(my_string and my_string.strip())


def benchmarkExists(spbench_path, selected_bench):
    """check if a given benchmark exists
    """
    registry_dic = getBenchRegistry(spbench_path)

    # Run through the registry dictionary elements
    for app_key in registry_dic:
        for ppi_key, value in registry_dic[app_key].items():
            for bench_key, value in registry_dic[app_key][ppi_key].items():
                if selected_bench == bench_key:
                    return True
    return False

def askToProceed():
    # user input support for python 2 and 3
    if(python_3 == 3):
        answer = input("\n Do you want to proceed? [yes/no]\n")
    else:
        answer = raw_input("\n Do you want to proceed? [yes/no]\n")

    if(answer.lower() not in ["y","yes"]): # delete the old benchmark
        print(" Operation canceled!\n")
        return False

    return True

def writeDicToBenchRegistry(spbench_path, registry_dic):
    """write dictionary to JSON registry file
    """
    with open(spbench_path + "/sys/registry.json", 'w') as f:
        json.dump(registry_dic, f, indent=4)
    f.close()

def writeDicToInputRegistry(spbench_path, registry_dic):
    """write dictionary to JSON inputs registry file
    """
    with open(spbench_path + "/sys/inputs_registry.json", 'w') as f:
        json.dump(registry_dic, f, indent=4)
    f.close()

def filterBenchRegByApp(registry_dic, selected_app):
    """Filter from a dictionary all keys that do not contain the given app
    """
    filtered_registry = {}
    benchmark_found = False

    for app_key in registry_dic:
        if app_key == selected_app:
            filtered_registry.update({app_key:registry_dic[app_key]})
            benchmark_found = True
    
    if not benchmark_found:
        print("\n  No benchmark found! Double check the name of the benchmark.\n")
        print(" You can run \'./spbench list\' to see all available benchmarks.")
        print(" You can also add new benchmarks using the command 'new'.")
        print(" Run \'./spbench new -help\' for more info.\n")
        sys.exit()
        
    return filtered_registry

def filterBenchRegByPPI(registry_dic, selected_ppi):
    """Filter from a dictionary all keys that do not contain the given PPI
    """
    filtered_registry = {}
    benchmark_found = False
    for app_key in registry_dic:
        for ppi_key, value in registry_dic[app_key].items():
            if ppi_key == selected_ppi:
                filtered_registry.update({app_key:{ppi_key:registry_dic[app_key][ppi_key]}})
                benchmark_found = True
    
    if not benchmark_found:
        print("\n  No benchmark found! Double check the name of the benchmark.\n")
        print(" You can run \'./spbench list\' to see all available benchmarks.")
        print(" You can also add new benchmarks using the command 'new'.")
        print(" Run \'./spbench new -help\' for more info.\n")
        sys.exit()

    return filtered_registry

def filterBenchRegByBench(registry_dic, selected_bench):

    filtered_registry = {}
    benchmark_found = False
    for app_key in registry_dic:
        for ppi_key, value in registry_dic[app_key].items():
            for bench_key in registry_dic[app_key][ppi_key]:
                if bench_key == selected_bench:
                    filtered_registry.update({app_key:{ppi_key:{bench_key:registry_dic[app_key][ppi_key][bench_key]}}})
                    benchmark_found = True

    if not benchmark_found:
        print("\n  No benchmark found! Double check the name of the benchmark.\n")
        print(" You can run \'./spbench list\' to see all available benchmarks.")
        print(" You can also add new benchmarks using the command 'new'.")
        print(" Run \'./spbench new -help\' for more info.\n")
        sys.exit()

    return filtered_registry

def filterRegistry(inputDictionary, args, copy_from = False):

    if(copy_from):
        bench_id = args.copy_from
    else:
        bench_id = args.benchmark_id
    # If no specific benchmark was given, check of other filters were used
    if not bench_id:
        # if only a app filter was selected
        if args.app_id and not args.ppi_id:
            selected_benchmarks = filterBenchRegByApp(inputDictionary, args.app_id)
            
        # if only a ppi filter was selected
        elif args.ppi_id and not args.app_id:
            selected_benchmarks = filterBenchRegByPPI(inputDictionary, args.ppi_id)
        # if both app and ppi filter were selected
        elif args.app_id and args.ppi_id:
            benchsFilteredByApp = filterBenchRegByApp(inputDictionary, args.app_id)
            selected_benchmarks = filterBenchRegByPPI(benchsFilteredByApp, args.ppi_id)
        # if no filter was selected, try to run for all
        else:
            print("\n Error! You must select some benchmark(s).\n")
            print(" To run this command for all benchmarks, use the argument: \'-bench all\'\n")
            print(" Run \'./spbench [command] --help\' to see all available options.\n")
            
            sys.exit()
            #selected_benchmarks = filterBenchRegByBench(inputDictionary, "all")
    else:
        if bench_id.lower() == 'all':
            selected_benchmarks = inputDictionary
        else:
        #    print("\n Warning! You selected all benchmarks for this procedure.\n")
        #    if not askToProceed():
        #        sys.exit()
            selected_benchmarks = filterBenchRegByBench(inputDictionary, bench_id)
    
    # check if dictionary is empty
    if not selected_benchmarks:
        print("\n  No benchmark found! Double check the name of the benchmark.\n")
        print(" You can run \'./spbench list\' to see all available benchmarks.")
        print(" You can also add new benchmarks using the command 'new'.")
        print(" Run \'./spbench new -help\' for more info.\n")
        sys.exit()

    return selected_benchmarks

def registryDicToList(benchs_dic):
    """Convert a benchmark dictionary to a list
    """
    benchmarks_list = [] # list of selected benchmarks to run

    for app_key in benchs_dic:
        for ppi_key, value in benchs_dic[app_key].items():
            for bench_key, value in benchs_dic[app_key][ppi_key].items():
                #TODO: add support to run by app or by PPI and differentiate nsource benchs
                benchmark_data = {
                    "app_id":app_key,
                    "ppi_id":ppi_key,
                    "bench_id":bench_key,
                    "nsources":False
                }
                if benchs_dic[app_key][ppi_key][bench_key] == "multiple":
                    benchmark_data["nsources"] = True

                benchmarks_list.append(benchmark_data)

    return benchmarks_list

def print_dic(registry_dic):
    """It prints a dictionary as a structured json
    """
    aux = json.dumps(registry_dic, indent=3)
    print(aux)
    return