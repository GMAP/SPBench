#!/usr/bin/env python3

import sys
import subprocess
import math

def runShellWithReturn(shell_cmd_line):
    """Run a shell command line and return the output from the command
    """
    try:
        output = subprocess.check_output(shell_cmd_line, stderr=subprocess.STDOUT, shell=True).decode()
        #success = True 
    except subprocess.CalledProcessError as e:
        output = e.output.decode() + "\n Unsuccessful execution\n"
        #print(output)
        #print("\n Unsuccessful execution\n")
    except Exception as e:
        # check_call can raise other exceptions, such as FileNotFoundError
        output = str(e) + "\n Unsuccessful execution\n"
        #print(output)
        #print("\n Unsuccessful execution\n")
    except KeyboardInterrupt as e:
        print(" KeyboardInterrupt")
        print("\n Unsuccessful execution\n")
        sys.exit()
    return(output)

def variance(data, ddof=0):
	n = len(data)
	mean = sum(data) / n
	return sum((x - mean) ** 2 for x in data) / (n - ddof)

def stdev(data):
	var = variance(data)
	std_dev = math.sqrt(var)
	return std_dev

def main():

    max_nthr = 25
    source_nth = "10"
    sink_nth = "5"
    repetitions = 5

    isPositiveFloat = lambda x: x.replace('.','',1).isdigit()

    cmd_line = "./bin/fd --rate 0 --keys 0 --sampling 100 --batch 0 --parallelism " # for fraud detection application, adapt it for your case

    nth_log_header = ("Thread Average_latency Std_dev_latency Average_throughput Std_dev_throughput Average_exec_time Std_dev_exec_time\n")

    output_file = "fd_" + source_nth + "-" + str(max_nthr) + "-" + sink_nth + ".dat" # output file name

    with open(output_file, 'w') as nth_log_file:
        nth_log_file.write(nth_log_header)

    for nthread in range(1, max_nthr+1):

        num_threads = source_nth + "," + str(nthread) + "," + sink_nth # for fraud detection application, adapt it for your case

        exec_line = cmd_line + num_threads

        latencies = []
        exec_times = []
        throughputs = []

        for n in range(0, repetitions):

            output = runShellWithReturn(exec_line)

            print(output)
            print("\n**************************************************\n\n")

            end_latency = 0
            exec_time = 0
            throughput = 0
            
            latency_average = 0
            thr_average = 0
            exec_time_average = 0
            exec_time_error = 0
            latency_error = 0
            thr_error = 0

            output_lines = output.splitlines()

            for line in output_lines: 
                if("End-to-end latency" in line): # the search keyword for the latency
                    end_latency = line.split()[2] # the value position in the line
                    if(isPositiveFloat(end_latency)):
                        latencies.append(float(end_latency))

                if("Execution time" in line): # the search keyword for the execution time
                    exec_time = line.split()[2] # the value position in the line
                    if(isPositiveFloat(exec_time)):
                        exec_times.append(float(exec_time))

                if("throughput" in line): # the search keyword for the throughput
                    throughput = line.split()[2] # the value position in the line
                    if(isPositiveFloat(throughput)):
                        throughputs.append(float(throughput))

        if(int(repetitions) > 1):

            if(latencies):
                latency_average = sum(latencies)/len(latencies)
                latency_error = stdev(latencies)

            if(exec_times):
                exec_time_average = sum(exec_times)/len(exec_times)
                exec_time_error = stdev(exec_times)

            if(throughputs):
                thr_average = sum(throughputs)/len(throughputs)
                thr_error = stdev(throughputs)

            with open(output_file, 'r+') as nth_log_file:
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

main()