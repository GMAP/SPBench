## 
 ##############################################################################
 #  File  : exec_option.py
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

def execute_func(spbench_path, args):

    inputs_registry_dic = getInputsRegistry(spbench_path) 

    # get list of selected benchmarks to run
    benchmarks_to_run = registryDicToList(filterRegistry(getBenchRegistry(spbench_path), args))

    # put user selected inputs into a list
    inputs_ID_list = []
    for sub_list in args.input_id:
        for input_id in sub_list:
            inputs_ID_list.append(input_id)
            
    
    for benchmark in benchmarks_to_run:
        app_id = benchmark["app_id"]
        ppi_id = benchmark["ppi_id"]
        bench_id = benchmark["bench_id"]
        nsources = benchmark["nsources"]

        if not fileExists(spbench_path + "/bin/" + app_id + "/" + ppi_id + "/" + bench_id):
            print("\n  Error!! Benchmark binary not found.")
            print("  Make sure you already compiled this benchmark.")
            print("\n  Try to run:\n    ./spbench compile -bench " + bench_id + "\n")

            sys.exit()
        
        inputs_dic = inputs_registry_dic[app_id]
        input_list = []
        for key in inputs_ID_list:
            if key not in inputs_dic:
                    print(" Input ID \'" + key + "\' not found for " + app_id)
                    print(" You can run \'./spbench list-inputs\' to see the registered inputs.")
                    print(" You can also run \'./spbench new-input -h\' to register a new input.")
                    continue
            if app_id == 'ferret' or app_id == 'person_recognition':
                input_list.append(inputs_dic[key]['input'].replace('$BENCH_DIR', spbench_path) + ' ' + key)
            else:
                input_list.append(inputs_dic[key]['input'].replace('$BENCH_DIR', spbench_path))

            # if it is a single source benchmark, get only the first input
            if not nsources:
                break
        
        # if list is empty, exit
        if not input_list:
            sys.exit()

        # Checking files for lane and bzip (there is no checking for ferret and person)
        if app_id == 'lane_detection' or app_id == 'bzip2':
            for input in input_list:
                if not fileExists(input): # check if the input file exists only for bzip and lane
                    print(" Input file " + input + " not found!")
                    continue

        input_id = ''
        for input in input_list: # generate a list of input files
            if(app_id == 'bzip2'):
                input_id += " " + os.path.abspath(input) # bzip2 does not require -i flag
            elif(app_id == 'ferret' or app_id == 'person_recognition'):
                input_id += " -i \"" + input + "\""   # ferret and person receive multiple files as workload
            else: # generate a '-i input_id_list' argument
                input_id += " -i " + os.path.abspath(input)
        
        # additional args
        other_args=''
        user_args = ''
        if app_id == 'bzip2':
            num_threads = " -t" + args.nthreads
            batch = " -b" + args.batch_size
            time_interval = ''
            other_args = ' --force --keep'
            if args.time_interval:
                time_interval = " -m" + args.time_interval

            items_reading_frequency = ''
            if args.items_reading_frequency:
                items_reading_frequency = " -F" + args.items_reading_frequency

            if(args.user_args):
                for sub_list in args.user_args:
                    for arg in sub_list:
                        user_args += " -u" + arg
        else:
            num_threads = " -t " + args.nthreads
            batch = " -b " + args.batch_size

            time_interval = ''
            if args.time_interval:
                time_interval = " -m " + args.time_interval

            items_reading_frequency = ''
            if args.items_reading_frequency:
                items_reading_frequency = " -F " + args.items_reading_frequency

            if(args.user_args):
                for sub_list in args.user_args:
                    for arg in sub_list:
                        user_args += " -u \"" + arg + "\""
            
        exec_arguments = " "
        if args.exec_arguments:
            exec_arguments = exec_arguments.join(args.exec_arguments) # join the input argument with the remaining arguments
        exec_arguments = input_id + num_threads + batch + time_interval + items_reading_frequency + " " + exec_arguments + other_args

        # build the execution command line and run it
        cmd_line = spbench_path + "/bin/" + app_id + "/" + ppi_id + "/" + bench_id + exec_arguments + user_args
        print("Execution string >> " + cmd_line)
        print("\n Running benchmark: " + bench_id)
        if not nsources:
            print("    Selected input: " + inputs_ID_list[0])

        else:
            print("   Selected inputs: ")
            for input_key in inputs_ID_list:
                print("       -> " + input_key)

        os.system(cmd_line)

        # result correctness checking
        if args.test_result:
            print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
            for input in inputs_ID_list:
                print("\n Checking result correctness for input \'" + input + '\'...\n')

                # check if there is a md5 hash associated to this input
                if not bool(inputs_registry_dic[app_id][input].get('md5_test')):
                    print(" -> Result checking unavailable for this input!")
                    print("\n - Make sure that input \'" + input + "\' has an expected\n   md5 hash associated to it.")
                    print("   Run \'./spbench list-inputs\' to check it.")
                    print("\n - Check the documentation for more info on how\n   to fix it (link)(forthcoming).")
                    print("\n - Or run \'./spbench edit_input -h\' to associate\n   an verification hash (forthcoming feature).")
                    print("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
                    continue
                
                checking_file = ''

                if app_id == 'bzip2':
                    # bzip2 output files can alse be used as workload for decompression.
                    # this way, the output files are in the same directory as input files and we must address this special case here
                    bzip2_out_file = (inputs_registry_dic[app_id][input]['input'].replace('$BENCH_DIR', spbench_path))
                    if '-d' in exec_arguments:
                        # if decompress is selected, remove the '.bz2'
                        checking_file = os.path.splitext(bzip2_out_file)[0]
                    else: # else, add a '.bz2'
                        checking_file = bzip2_out_file + ".bz2"
                
                elif app_id == 'ferret':
                    # ferret output is disordered, so it requires sorting the output
                    output_file = spbench_path + "/outputs/" + bench_id + '_' + input + ".out"

                    # check if output exists before sorting it
                    if not outputExists(output_file): continue 

                    checking_file = spbench_path + "/outputs/" + bench_id + '_' + input + "_sorted.out"
                    sorting_cmd = "sort " + output_file + " > " + checking_file
                    os.system(sorting_cmd)

                else: # lane and person
                    checking_file = spbench_path + "/outputs/" + bench_id + '_' + input + ".avi"

                # also check the ouput for bzip2, person, and lane
                if not outputExists(checking_file): continue

                # compute md5 hash for the output file    
                resulting_md5 = md5(checking_file)

                if app_id == 'ferret': # deleting ferret's sorted output
                    os.system('rm ' + checking_file)
                
                # compare the computed hash to the one stored in the inputs database
                if resulting_md5 == inputs_registry_dic[app_id][input]['md5_test']:
                    print(" -> Correct!")
                    print(" - Expected md5 hash:  " + inputs_registry_dic[app_id][input]['md5_test'])
                    print(" - Resulting md5 hash: " + resulting_md5)
                else:
                    print(" -> Incorrect!")
                    print(" - Expected md5 hash:  " + inputs_registry_dic[app_id][input]['md5_test'])
                    print(" - Resulting md5 hash: " + resulting_md5)
                print("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
            print("")

    sys.exit()

           
