## 
 ##############################################################################
 #  File  : exec_option.py
 #
 #  Title : SPBench-CLI Benchmark Execution Option
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
import datetime

from src.errors import *
from src.utils import *

from sys import version_info 
python_3 = version_info[0]

def execute_func(spbench_path, args):
    
    is_range = False
    nthreads = ["1"]

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
            print("\n  Error!! Binary file not found for "+ bench_id)
            print("  Make sure you compiled the benchmark before run it.")
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
                input_list.append(inputs_dic[key]['input'].replace('$SPB_HOME', spbench_path) + ' ' + key)
            else:
                input_list.append(inputs_dic[key]['input'].replace('$SPB_HOME', spbench_path))

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

        # Check for errors in nthreads
        if args.nthreads:
            # check if it is a range or not
            if args.nthreads.isdigit():
                nthreads[0] = args.nthreads
            else:
                # check format and get the range values
                nthreads = args.nthreads.replace(" ", "").lower()
                # check if it the range number of parameters
                if len(nthreads.split(':')) < 2 or len(nthreads.split(':')) > 3:
                    raise ArgumentTypeError("Argument error! Invalid format for nthreads range: " + nthreads + "\n  Expected: -nthreads start:end or start:step:end\n\n")

                range_arg = nthreads.split(':')
                for value in range_arg:
                    if not value.isdigit():
                        raise ArgumentTypeError("Argument error! Values in the range must be integer numbers higher than or equal to one: " + args.nthreads)

                range_start = int(range_arg[0])
                if len(range_arg) == 2:
                    range_step = 1
                    range_end = int(range_arg[1])
                elif len(range_arg) == 3:
                    range_step = int(range_arg[1])
                    range_end = int(range_arg[2])
                else:
                    raise ArgumentTypeError("Argument error! Invalid format for nthreads range: " + nthreads + "\n  Expected: -nthreads start:end or start:step:end\n\n")

                if range_start < 1 or range_end < 1:
                    raise ArgumentTypeError("Argument error! Values in the range must be integer numbers higher than or equal to one: " + args.nthreads)

                if range_start <= range_end:
                    nthreads = range((range_start), (range_end)+1, (range_step))
                if range_start > range_end:
                    nthreads = range((range_start), (range_end)-1, -abs((range_step)))

                is_range = True

        # Check for errors in frequency pattern
        frequency_pattern = ''
        isPositiveFloat = lambda x: x.replace('.','',1).isdigit()
        if args.frequency_pattern:
            frequency_pattern = args.frequency_pattern.replace(" ", "").lower()
            
            # check if it has the righ number of parameters
            if len(frequency_pattern.split(',')) < 4 or len(frequency_pattern.split(',')) > 5:
                raise ArgumentTypeError("Argument error! Invalid format for frequency pattern: " + frequency_pattern + "\n  Expected: -freq-pattern pattern,period,min,max,spike\n\n  Obs.: Spike argument is optional, it only aplies for the spike pattern.")

            pattern = frequency_pattern.split(',')[0]
            patterns = ['wave', 'spike', 'binary', 'increasing', 'decreasing']
            if pattern not in patterns:
                raise ArgumentTypeError("Argument error! Invalid pattern for frequency: " + pattern + "\n  You must select from: " + str(patterns))
                
            period = frequency_pattern.split(',')[1]
            min_freq = frequency_pattern.split(',')[2]
            max_freq = frequency_pattern.split(',')[3]

            if not isPositiveFloat(period) or not isPositiveFloat(min_freq) or not isPositiveFloat(max_freq):
                raise ArgumentTypeError("Argument error! Invalid values for period, or maximum or minimum frequency: " + frequency_pattern)

            # spike checking
            spike_p = 0.0
            if pattern == "spike":
                spike_p = float(period)/10
                if len(frequency_pattern.split(',')) == 5:
                    spike_p = frequency_pattern.split(',')[4]
                    if not isPositiveFloat(spike_p):
                        raise ArgumentTypeError("Argument error! Invalid value for spike period: " + frequency_pattern)
                    if float(spike_p) > 100:
                        raise ArgumentTypeError("Argument error! Invalid value for spike period: " + frequency_pattern + "\n  It must be a value between 0 and 100 (it is defined as percentage of the period).")
                    spike_p = float(period)/float(frequency_pattern.split(',')[4])
                    
            if float(min_freq) > float(max_freq):
                raise ArgumentTypeError("Argument error! Maximum frequency must be higher than minimum frequency: " + frequency_pattern)

        # Check for errors in repetitions
        if args.repetitions:
            if not args.repetitions.isdigit():
                raise ArgumentTypeError("Argument error! The number of repetitions must be an integer higher than or equal to one: " + args.repetitions)

        # Check for errors in batch size
        if args.batch_size:
            if not args.batch_size.isdigit():
                raise ArgumentTypeError("Argument error! Batch size must be an integer number higher than or equal to one: " + args.batch_size)

        # Check for errors in batch interval
        if args.batch_interval:
            if not isPositiveFloat(args.batch_interval):
                raise ArgumentTypeError("Argument error! Batch interval window must be a number higher than or equal to zero: " + args.batch_interval)
        
        if nsources and args.time_interval_thr:
            raise ArgumentTypeError("Argument error! -monitor-thread feature is not available for multi-source benchmarks. You can use the -monitor instead!")

        if args.time_interval and args.time_interval_thr:
            raise ArgumentTypeError("Argument error! You can not use both -monitor and -monitor-thread arguments at once!")
        else:
            # Check for errors in monitoring interval
            if args.time_interval:
                if not args.time_interval.isdigit():
                    raise ArgumentTypeError("Argument error! Time interval for monitoring must be an integer number higher than or equal to one: " + args.time_interval)
            
            # Check for errors in monitoring interval
            if args.time_interval_thr:
                if not args.time_interval_thr.isdigit():
                    raise ArgumentTypeError("Argument error! Time interval for monitoring must be an integer number higher than or equal to one: " + args.time_interval_thr)
        

        # Check for errors in frequency
        if args.items_frequency:
            if not isPositiveFloat(args.items_frequency):
                raise ArgumentTypeError("Argument error! value set for frequency must be a positive number: " + args.items_frequency)

        # translate CLI arguments sintax to SPBench's applications argument sintax
        other_args=''
        user_args = ''
        num_threads = ''
        if app_id == 'bzip2':
            #if args.nthreads:
            #    num_threads = " -t" + args.nthreads

            batch_size = ''
            if args.batch_size:
                batch_size = " -b" + args.batch_size

            batch_interval = ''
            if args.batch_interval:
                batch_interval = " -B" + args.batch_interval

            time_interval = ''
            if args.time_interval_thr:
                time_interval = " -M" + args.time_interval_thr
                
            if args.time_interval:
                time_interval = " -m" + args.time_interval

            items_frequency = ''
            if args.items_frequency:
                items_frequency = " -F" + args.items_frequency

            if frequency_pattern:
                frequency_pattern = " -p" + frequency_pattern

            if(args.user_args):
                for sub_list in args.user_args:
                    for arg in sub_list:
                        user_args += " -u" + arg
            
            other_args = ' --force --keep'

        else:
            #if args.nthreads:
            #    num_threads = " -t " + args.nthreads

            batch_size = ''
            if args.batch_size:
                batch_size = " -b " + args.batch_size

            batch_interval = ''
            if args.batch_interval:
                batch_interval = " -B " + args.batch_interval

            time_interval = ''
            if args.time_interval_thr:
                time_interval = " -M " + args.time_interval_thr
            if args.time_interval:
                time_interval = " -m " + args.time_interval

            items_frequency = ''
            if args.items_frequency:
                items_frequency = " -F " + args.items_frequency

            if frequency_pattern:
                frequency_pattern = " -p " + frequency_pattern

            if(args.user_args):
                for sub_list in args.user_args:
                    for arg in sub_list:
                        user_args += " -u \"" + arg + "\""
            
        exec_arguments = " "
        if args.exec_arguments:
            exec_arguments = exec_arguments.join(args.exec_arguments) # join the input argument with the remaining arguments

        exec_arguments = input_id + batch_size + batch_interval + time_interval + items_frequency + frequency_pattern + " " + exec_arguments + other_args

        # build the execution command line and run it
        cmd_line = spbench_path + "/bin/" + app_id + "/" + ppi_id + "/" + bench_id + exec_arguments + user_args

        if(args.executor):
            cmd_line = args.executor + " " + cmd_line

        if(args.print_exec_line):
            if app_id == 'bzip2':
                if is_range:
                    num_threads = " -t" + str(range_end)
                else:
                    num_threads = " -t" + args.nthreads
            else:
                if is_range:
                    num_threads = " -t " + str(range_end)
                else:
                    num_threads = " -t " + args.nthreads

            print(cmd_line + num_threads)
            continue

        #print("Execution string >> " + cmd_line)
        print("\n Running benchmark: " + bench_id)
        if not nsources and not args.quiet:
            print("    Selected input: " + inputs_ID_list[0])
            if batch_size:
                print("        Batch size: " + args.batch_size + " items per batch")
            if batch_interval:
                print("    Batch interval: " + args.batch_interval + " milliseconds")
            if args.exec_arguments and "-k" in args.exec_arguments:
                print("    In-memory mode: enabled")
            else:
                print("    In-memory mode: not enabled")
            if frequency_pattern:
                if(pattern == 'wave'):
                    print("\n Frequency pattern: wave\n")
                    print("     #  #                  #  #  - - - - - - - -> Maximum: " + max_freq + " items per second")
                    print("  #        #            #        #                    ")
                    print("             #        #            #        #          ")
                    print("                #  #                  #  # - - -> Minimum: " + min_freq + " items per second")
                    print("                        |____________________| -> Periods: " + period + " seconds")
                elif(pattern == 'binary'):
                    print("\n Frequency pattern: binary\n")
                    print("         # # # # #       # # # #  - -> Maximum: " + max_freq + " items per second")
                    print("         #       #       #           ")
                    print("         #       #       #           ")
                    print(" # # # # #       # # # # #  - - - - -> Minimum: " + min_freq + " items per second")
                    print("                 |______________| - -> Periods: " + period + " seconds")
                elif(pattern == 'spike'):
                    print("\n Frequency pattern: spike\n")
                    print("            #            #  - -> Maximum: " + max_freq + " items per second")
                    print("           ##           ##   ")
                    print("          # #          # #   ")
                    print(" # # # # #  # # # # # #  # # #-> Minimum: " + min_freq + " items per second")
                    print("                     |____| - -> Spikes:  " + str(spike_p) + " seconds")
                    print("            |_____________| - -> Periods: " + period + " seconds")
                elif(pattern == 'increasing'):
                    print("\n Frequency pattern: increasing\n")
                    print("          # # # # # # # ...  -> Maximum: " + max_freq + " items per second")
                    print("       #")
                    print("    #")
                    print(" # - - - - - - - - - - - - - -> Minimum: " + min_freq + " items per second")
                    print("|__________| - - - - - - - - -> Period:  " + period + " seconds")
                elif(pattern == 'decreasing'):
                    print("\n Frequency pattern: decreasing\n")
                    print(" # - - - - - - - - - - - - - -> Maximum: " + max_freq + " items per second")
                    print("    #")
                    print("       #")
                    print("          # # # # # # # ...  -> Minimum: " + min_freq + " items per second")
                    print("|__________| - - - - - - - - -> Period:  " + period + " seconds")
                else:
                    print("Invalid frequency pattern:", pattern)
                    sys.exit()
            elif args.items_frequency:
                print("  Target frequency: " + args.items_frequency)
        else:
            if not args.quiet:
                print("   Selected inputs: ")
                for input_key in inputs_ID_list:
                    print("       -> " + input_key)
        #print('\n')

        latencies = []
        throughputs = []
        exec_times = []

        # prepare the performance log
        log_dir = spbench_path + "/log"
        if is_range and (int(args.repetitions) > 1):
            range_log_file = log_dir + "/" + bench_id + "_" + str(range_start) + "-" + str(range_step) + "-" + str(range_end) + ".dat"
            nth_log_header = ("Thread Average_latency Std_dev_latency Average_throughput Std_dev_throughput Average_exec_time Std_dev_exec_time\n")

            #if(not fileExists(range_log_file)):
            with open(range_log_file, 'w') as nth_log_file:
                nth_log_file.write(nth_log_header)

        # prepare the general execution log
        log_header = ("Time;Benchmark;Latency;Throughput;Exec. time;Max lat.;Min lat.;Input;N threads;Batch size;Batch int.;Frequency;Freq. patt.\n")
        log_file = log_dir + "/general_log.csv"

        if(not dirExists(log_dir)):
            try: 
                os.mkdir(log_dir) 
            except OSError as error: 
                print(error)
        
        if(not fileExists(log_file)):
            with open(log_file, 'w') as general_log_file:
                general_log_file.write(log_header)

        # run the benchmark for n threads
        for nthread in nthreads:

            # add nthread parameter to the execution line
            if app_id == 'bzip2':
                num_threads = " -t" + str(nthread)
            else:
                num_threads = " -t " + str(nthread)
            
            exec_line = cmd_line + num_threads

            if is_range:
                print("\n ~~~> Running " + bench_id + " with " + str(nthread) + " threads!")

            latencies = []
            exec_times = []
            throughputs = []

            # run the benchmark n times
            for n in range(0, int(args.repetitions)):

                if(int(args.repetitions) > 1):
                    print("\n ~~~~~~> Execution " + str(n+1) + " from " + args.repetitions)

                # run the command line
                output = runShellWithReturn(exec_line)

                if not args.quiet:
                    print(output)

                end_latency = 0
                max_latency = 0
                min_latency = 0
                exec_time = 0
                throughput = 0

                output_lines = output.splitlines()
                for line in output_lines:
                    
                    if("End-to-end latency" in line):
                        end_latency = line.split()[4]
                        if(isPositiveFloat(end_latency)):
                            latencies.append(float(end_latency))

                    if("Maximum latency" in line):
                        max_latency = line.split()[4]

                    if("Minimum latency" in line):
                        min_latency = line.split()[4]
                    
                    if("Execution time" in line):
                        exec_time = line.split()[4]
                        if(isPositiveFloat(exec_time)):
                            exec_times.append(float(exec_time))
                    
                    if("Items-per-second" in line):
                        throughput = line.split()[2]
                        if(isPositiveFloat(throughput)):
                            throughputs.append(float(throughput))

                if args.quiet:
                    if '-l' in args.exec_arguments:
                        print(" Average latency (ms) = " + str((round(float(end_latency), 3))))
                    if '-x' in args.exec_arguments:
                        print("     Items per second = " + str((round(float(throughput), 3))))
                        print(" Execution time (sec) = " + str((round(float(exec_time), 3))))
                ##
                # Writing results to the general log file
                ##
                time_now = datetime.datetime.now()
                print_time = time_now.strftime("%d/%m/%y %H:%M:%S")
                log_line = []
                log_line.append(str(print_time))
                log_line.append(bench_id)
                log_line.append(str((round(float(end_latency), 3))))
                log_line.append(str(round(float(throughput), 3)))
                log_line.append(str(round(float(exec_time), 3)))
                log_line.append(str(round(float(max_latency), 3)))
                log_line.append(str(round(float(min_latency), 3)))
                log_line.append(','.join(inputs_ID_list))
                log_line.append(str(nthread))
                log_line.append(args.batch_size)
                log_line.append(args.batch_interval)
                log_line.append(args.items_frequency)
                log_line.append(args.frequency_pattern + "\n")

                with open(log_file, 'r+') as general_log_file:
                    general_log_file.seek(0)
                    current_header = general_log_file.readline()

                    if(log_header not in current_header):
                        general_log_file.seek(0)
                        general_log_file.write(log_header)

                    general_log_file.seek(0, 2)
                    general_log_file.write(';'.join(log_line))
                    general_log_file.truncate()
                
                ##
                # result correctness checking
                ##
                if args.test_result:
                    print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
                    for input in inputs_ID_list:
                        if not args.quiet:
                            print("\n Checking result correctness for input \'" + input + '\'...\n')

                        # check if there is a md5 hash associated to this input
                        if not bool(inputs_registry_dic[app_id][input].get('md5_test')):
                            print(" -> Result checking unavailable for this input!")
                            if not args.quiet:
                                print("\n - Make sure that input \'" + input + "\' has an expected\n   md5 hash associated to it.")
                                print("   Run \'./spbench list-inputs\' to check it.")
                                print("\n - Check the documentation for more info on how\n   to fix it (link)(forthcoming).")
                                print("\n - Or run \'./spbench edit_input -h\' to associate\n   an verification hash (forthcoming feature).")
                            print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
                            continue
                        
                        checking_file = ''

                        if app_id == 'bzip2':
                            # bzip2 output files can alse be used as workload for decompression.
                            # this way, the output files are in the same directory as input files and we must address this special case here
                            bzip2_out_file = (inputs_registry_dic[app_id][input]['input'].replace('$SPB_HOME', spbench_path))
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
                            print(" -> SUCCESS!")
                        else:
                            print(" -> UNSUCCESSFUL!\n")
                            if not args.quiet:
                                print(" Incorrect output")
                                print(" - Expected md5 hash:  " + inputs_registry_dic[app_id][input]['md5_test'])
                                print(" - Resulting md5 hash: " + resulting_md5)
                        print("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
                    print("")
                # end of the correcteness checking

            ##
            # Compute and print the metrics sumary
            ##
            if(int(args.repetitions) > 1):

                if(latencies):
                    latency_average = sum(latencies)/len(latencies)
                    latency_error = stdev(latencies)

                if(exec_times):
                    exec_time_average = sum(exec_times)/len(exec_times)
                    exec_time_error = stdev(exec_times)

                if(throughputs):
                    thr_average = sum(throughputs)/len(throughputs)
                    thr_error = stdev(throughputs)

                if (latencies or exec_times or throughputs) and not args.quiet:
                    print("*************** RESULTS SUMARY ***************\n")
                    print("             Benchmark:", bench_id)
                    print("           Repetitions:", args.repetitions)
                    if(latencies):
                        print("\n       Average latency:", latency_average)
                        print("     Latency std. dev.:", latency_error)
                    if(throughputs):
                        print("\n    Average throughput:", thr_average)
                        print("  Throughput std. dev.:", thr_error)
                    if(exec_times):
                        print("\n    Average exec. time:", exec_time_average)
                        print("  Exec. time std. dev.:", exec_time_error)

                    if nsources:
                        print("\n CAUTION: This is a multi-source benchmark.")
                        print("          This summary includes all different")
                        print("          sources and may not be accurate.")
                    print("\n********************************************")
            
                ##
                # Generate a specific performance log if repetitions and nthreads range are enabled
                ##
                if is_range:
                    with open(range_log_file, 'r+') as nth_log_file:
                        nth_log_file.seek(0)
                        current_header = nth_log_file.readline()

                        if(nth_log_header not in current_header):
                            nth_log_file.seek(0)
                            nth_log_file.write(nth_log_header)

                        nth_log_file.seek(0, 2)
                        nth_log_file.write(
                            str(nthread) + " " + 
                            str(latency_average) + " " + 
                            str(latency_error) + " " + 
                            str(thr_average) + " " + 
                            str(thr_error) + " " + 
                            str(exec_time_average) + " " + 
                            str(exec_time_error) + "\n")
                        nth_log_file.truncate()

    sys.exit()
