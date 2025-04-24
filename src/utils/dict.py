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

import sys
import os
import json

from . import utils
from .shell import *
from difflib import SequenceMatcher

spbench_path_ = os.path.abspath(os.path.dirname(os.path.realpath(__file__)) + "/../../")

def getBenchRegistry(spbench_path):
    """
    return a dictionay with the benchmarks registered data
    """
    registry_file = spbench_path + "/benchmarks/benchmarks_registry.json"
    #check if the registry exists
    if os.path.exists(registry_file) == False:
        print("\n There is no benchmarks registered.\n Registry file not found at " + registry_file + "\n")
        sys.exit()
    return getDictFromJSON(registry_file)

def getInputsRegistry(spbench_path):
    """
    return a dictionay with the inputs registered data
    """
    registry_file = spbench_path + "/inputs/inputs_registry.json"
    #check if the registry exists
    if os.path.exists(registry_file) == False:
        print("\n There is no inputs registered.\n Registry file not found at " + registry_file + "\n")
        sys.exit()
    return getDictFromJSON(registry_file)

def getDependenciesRegistry(spbench_path):
    """return a dictionay with the dependencies registered in SPBench/libs/dependecies.json
    """
    registry_file = spbench_path + "/libs/dependencies.json"
    #check if the registry exists
    if os.path.exists(registry_file) == False:
        print("\n There is no dependencies registered.\n Registry file not found at " + registry_file + "\n")
        sys.exit()
    return getDictFromJSON(registry_file)

def getAppsRegistry(spbench_path):
    """
    return a dictionay with the apps registered data
    """
    registry_file = spbench_path + "/sys/apps/apps_registry.json"
    #check if the registry exists
    if os.path.exists(registry_file) == False:
        print("\n There is no apps registered.\n Registry file not found at " + registry_file + "\n")
        sys.exit()
    return getDictFromJSON(registry_file)

def getAppsList(spbench_path):
    """
    return a list with the apps in the json registry
    """
    registry_dic = getAppsRegistry(spbench_path)
    apps_list = []
    for app_key in registry_dic:
        apps_list.append(app_key)
    return apps_list

def getAppsListAll(spbench_path):
    """
    return a list with the allowed apps names
    """
    apps_list = getAppsList(spbench_path)
    apps_list_all = list(apps_list)
    apps_list_all.insert(0, 'all')
    return apps_list_all

def deleteAppFromRegistry(spbench_path, app_id):
    """
    delete a app from the registry and sys/apps/ folder
    """
    # delete app key from the app registry
    apps_registry = getAppsRegistry(spbench_path)
    del apps_registry[app_id]
    writeDicTo(spbench_path + "/sys/apps/apps_registry.json", apps_registry)

    return

def getDictFromJSON(registry_file):
    """
    Reads the content of a JSON file and returns its content as a dictionary.

    Args:
        registry_file (str): Path to the JSON file.

    Returns:
        dict: Dictionary representation of the JSON file's content.

    Raises:
        FileNotFoundError: If the file does not exist.
        json.JSONDecodeError: If the file content is not valid JSON.
        Exception: For any other errors that might occur.
    """
    try:
        with open(registry_file, 'r') as f:
            registry_dic = json.load(f)
        return registry_dic
    except FileNotFoundError:
        raise FileNotFoundError(f"File not found: {registry_file}")
    except json.JSONDecodeError as e:
        raise ValueError(f"Error decoding JSON from file {registry_file}: {e}")
    except Exception as e:
        raise RuntimeError(f"An unexpected error occurred while reading the file {registry_file}: {e}")

# check if a given app exists
def appExists(spbench_path, selected_app):
    """
    check if a given app exists
    """
    registry_dic = getBenchRegistry(spbench_path)

    # Run through the registry dictionary elements
    for app_key in registry_dic:
        if selected_app == app_key:
            return True
    return False

# check if a given benchmark exists
def benchmarkExists(spbench_path, selected_bench):
    """
    check if a given benchmark exists
    """
    registry_dic = getBenchRegistry(spbench_path)

    # Run through the registry dictionary elements
    for app_key in registry_dic:
        for ppi_key, value in registry_dic[app_key].items():
            for bench_key, value in registry_dic[app_key][ppi_key].items():
                if selected_bench == bench_key:
                    return True
    return False

def writeDicTo(registry_file, registry_dic):
    """
    write dictionary to JSON registry file
    """
    with open(registry_file, 'w') as f:
        json.dump(registry_dic, f, indent=4)
    f.close()

def writeDicToBenchRegistry(spbench_path, registry_dic):
    """
    write dictionary to JSON registry file
    """
    with open(spbench_path + "/benchmarks/benchmarks_registry.json", 'w') as f:
        json.dump(registry_dic, f, indent=4)
    f.close()

def writeDicToInputRegistry(spbench_path, registry_dic):
    """
    write dictionary to JSON inputs registry file
    """
    with open(spbench_path + "/inputs/inputs_registry.json", 'w') as f:
        json.dump(registry_dic, f, indent=4)
    f.close()

def filterBenchRegByApp(registry_dic, selected_app, runDoYouMean = True):
    """
    Filter from a dictionary all keys that do not contain the given app
    """
    filtered_registry = {}
    benchmark_found = False

    for app_key in registry_dic:
        if app_key == selected_app:
            filtered_registry.update({app_key:registry_dic[app_key]})
            benchmark_found = True
    
    if not benchmark_found and runDoYouMean:
        print("\n  Application not found! " + doYouMeanPPI(selected_app))
        print("\n  Double check the name of the application.")
        print(" You can run \'./spbench list\' to see all available applications.")
        print(" You can also add new benchmarks using the command 'new'.")
        print(" Run \'./spbench new -help\' for more info.\n")
        sys.exit()
        
    return filtered_registry

def filterBenchRegByPPI(registry_dic, selected_ppi, runDoYouMean = True):
    """Filter from a dictionary all keys that do not contain the given PPI
    """
    filtered_registry = {}
    benchmark_found = False
    for app_key in registry_dic:
        for ppi_key, value in registry_dic[app_key].items():
            if ppi_key == selected_ppi:
                filtered_registry.update({app_key:{ppi_key:registry_dic[app_key][ppi_key]}})
                benchmark_found = True
    
    if not benchmark_found and runDoYouMean:
        print("\n  PPI not found! " + doYouMeanPPI(selected_ppi))
        print("\n  Double check the name of the PPI.")
        print("  You can run \'./spbench list\' to see all available PPIs.")
        print("  You can also add new benchmarks using the command 'new'.")
        print("  Run \'./spbench new -help\' for more info.\n")
        sys.exit()

    return filtered_registry

def filterBenchRegByBench(registry_dic, selected_bench, runDoYouMean = True):

    filtered_registry = {}
    benchmark_found = False
    for app_key in registry_dic:
        for ppi_key, value in registry_dic[app_key].items():
            for bench_key in registry_dic[app_key][ppi_key]:
                if bench_key == selected_bench:
                    filtered_registry.update({app_key:{ppi_key:{bench_key:registry_dic[app_key][ppi_key][bench_key]}}})
                    benchmark_found = True

    if not benchmark_found and runDoYouMean:
        print("\n  Benchmark not found! " + doYouMeanBench(selected_bench))
        print("\n  Double check the name of the benchmark.")
        print("  You can run \'./spbench list\' to see all available benchmarks.")
        print("  You can also add new benchmarks using the command 'new'.")
        print("  Run \'./spbench new -help\' for more info.\n")
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
            print("  To run this command for all benchmarks, use the argument: \'-bench all\'\n")
            print("  Run \'./spbench [command] --help\' to see all available options.\n")
            
            sys.exit()
    else:
        if bench_id.lower() == 'all':
            selected_benchmarks = inputDictionary
        else:
            selected_benchmarks = filterBenchRegByBench(inputDictionary, bench_id)
    
    # check if dictionary is empty
    if not selected_benchmarks:
        print("\n  No benchmark found! Double check the name of the benchmark.\n")
        print("  You can run \'./spbench list\' to see all available benchmarks.")
        print("  You can also add new benchmarks using the command 'new'.")
        print("  Run \'./spbench new -help\' for more info.\n")
        sys.exit()

    return selected_benchmarks

def registryDicToList(benchs_dic):
    """
    Convert a benchmark dictionary to a list
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
    """
    It prints a dictionary as a structured json
    """
    aux = json.dumps(registry_dic, indent=3)
    print(aux)
    return

def doYouMeanBench(benchmark):
    """
    check if there is a benchmark with similar name
    """
    registry_dic = getBenchRegistry(spbench_path_)

    most_similar_score = 0.0
    most_similar_key = ""
    # Run through the registry dictionary elements
    for app_key in registry_dic:
        for ppi_key, value in registry_dic[app_key].items():
            for bench_key, value in registry_dic[app_key][ppi_key].items():
                similarity_score = SequenceMatcher(None, benchmark, bench_key).ratio()
                if similarity_score > most_similar_score:
                    most_similar_score = similarity_score
                    most_similar_key = bench_key

    if most_similar_score > 0.0:
        return "Do you mean \"" + most_similar_key + "\"?"
    return ''

def doYouMeanApp(app_name):
    """
    check if there is an application with similar name
    """
    registry_dic = getBenchRegistry(spbench_path_)

    most_similar_score = 0.0
    most_similar_key = ""
    # Run through the registry dictionary elements
    for app_key in registry_dic:
        similarity_score = SequenceMatcher(None, app_name, app_key).ratio()
        if similarity_score > most_similar_score:
            most_similar_score = similarity_score
            most_similar_key = app_key

    if most_similar_score > 0.0:
        return "Do you mean \"" + most_similar_key + "\"?"
    return ''

def doYouMeanPPI(ppi_name):
    """
    check if there is a PPI with similar name
    """
    registry_dic = getBenchRegistry(spbench_path_)

    most_similar_score = 0.0
    most_similar_key = ""
    for app_key in registry_dic:
        for ppi_key, value in registry_dic[app_key].items():
            similarity_score = SequenceMatcher(None, ppi_name, ppi_key).ratio()
            if similarity_score > most_similar_score:
                most_similar_score = similarity_score
                most_similar_key = ppi_key

    if most_similar_score > 0.25:
        return "Do you mean \"" + most_similar_key + "\"?"
    return ''