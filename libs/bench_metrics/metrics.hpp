/** 
 * ************************************************************************  
 *  File  : metrics.hpp
 *
 *  Title : SPBench metrics library
 *
 *  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 *
 *  Date  : July 06, 2021
 *
 *  Copyright (C) 2021 Adriano M. Garcia
 * 
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License version 3 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 ****************************************************************************
 */

#include <vector>
#include <string>
#include <iostream>
#include <sys/time.h>
#include <fstream>
#include <upl.h>

using namespace std;

struct monitor_data;
struct latency_t;
struct data_metrics;

inline void monitor_metrics(volatile unsigned long item_timestamp);
inline bool file_exists (const std::string& name);
inline data_metrics init_metrics();
inline void stop_metrics(data_metrics metrics);
std::vector<string> set_operators_name();
volatile unsigned long current_time_usecs();
void print_throughput(data_metrics metrics);
void print_average_latency();
void write_latency(std::string file_name);
std::string version_name;
std::string out_file_path(std::string out_paths);


unsigned long item_count = 0;

unsigned long output_items = 0;
unsigned long input_items = 0;

volatile unsigned long start_time_usec;
volatile unsigned long end_time_usec;

volatile unsigned long execution_init_clock;
volatile unsigned long item_old_time;

unsigned long monitoring_time_interval = 250;
unsigned long items_reading_frequency = 0; //usec precision

unsigned int batch_size = 1;
unsigned int nthreads = 1;

bool stream_end = false;
bool print_latency = false;
bool latency_to_file = false;
bool enable_upl = false;
bool enable_throughput = false;
bool enable_monitoring = false;
bool memory_source = false;

//to store the individual latencies
std::vector<latency_t> latency_vector;
std::vector<monitor_data> monitor_vector;

std::string filepath; //stores executable name from arg[0] in init_bench()

//parser to retrieve the executable name from the full path
template<class T>
T base_name(T const & path, T const & delims = "//")
{
  return path.substr(path.find_last_of(delims) + 1);
}
/*template<class T>
T remove_extension(T const & filename)
{
  typename T::size_type const p(filename.find_last_of('.'));
  return p > 0 && p != T::npos ? filename.substr(0, p) : filename;
}*/

struct monitor_data{
	float timestamp;
	float cpu_usage;
	ssize_t mem_usage;
	float latency_item;
	float throughput;
};

struct latency_t {
	std::vector<double> local_latency;
	double local_total;
	latency_t():
		local_total(0.0)
	{}
};

struct data_metrics {
		int *rapl_fd;
		int fd_cache;
		volatile unsigned long start_throughput_clock;
		volatile unsigned long stop_throughput_clock;
	data_metrics():
		rapl_fd(NULL),
		fd_cache(0),
		start_throughput_clock(0),
		stop_throughput_clock(0)
	{}
};

volatile unsigned long current_time_usecs() {
	struct timeval time;
	gettimeofday(&time, NULL);
	return time.tv_sec * 1000000 + time.tv_usec;
}

std::string out_file_path(std::string out_paths){
	if(!file_exists(out_paths)){
			if (mkdir(out_paths.c_str(), 0777) != 0){
        		cerr << "Error :  " << strerror(errno) << endl;
			}
	}
	return (out_paths + "/" + base_name(filepath));
}

inline void monitor_metrics(volatile unsigned long item_timestamp){
	item_count++;
	monitor_data item_data;
	float elapsed_time = (current_time_usecs() - item_old_time)/1000.0;
	if(elapsed_time >= monitoring_time_interval){ //Time interval ms
		item_old_time = current_time_usecs();
		item_data.latency_item = ((current_time_usecs() - item_timestamp) / 1000.0);
		item_data.timestamp = (current_time_usecs() - execution_init_clock)/1000000.0;
		item_data.cpu_usage = UPL_get_proc_load_average_now(UPL_getProcID());
		item_data.mem_usage = UPL_getProcMemUsage();
		item_data.throughput = item_count / item_data.timestamp;
		monitor_vector.push_back(item_data);
		//printf("%.4f %.4f %ld %.4f %.4f\n", item_data.timestamp, item_data.cpu_usage, item_data.mem_usage, item_data.throughput, item_data.latency_item);
	}

}

inline bool file_exists (const std::string& name) {
    ifstream f(name.c_str());
    return f.good();
}

/* Item frequency control mechanism */
inline void item_frequency_control(volatile unsigned long item_timestamp){
	while(1){
		//float elapsed_time_usec = ((current_time_usecs() - elapsed_time_usec));
		if((current_time_usecs() - item_timestamp) >= items_reading_frequency){
			break;
		}
	}
}

inline data_metrics init_metrics(){

	execution_init_clock = current_time_usecs();
	data_metrics metrics;
	if(enable_upl){
		if(UPL_init_cache_miss_monitoring(&metrics.fd_cache) == 0){
			std::cout << "Error when UPL_init_cache_miss_monitoring(...)" << std::endl;
		}

		metrics.rapl_fd = new int[4];
		if(UPL_init_count_rapl(metrics.rapl_fd) == 0){
			std::cout << "Error when UPL_init_count_rapl(...)" << std::endl;
		}
	}
	if(enable_throughput){
		metrics.start_throughput_clock = current_time_usecs();
	}
	item_old_time = current_time_usecs();
	return metrics;
}

inline void stop_metrics(data_metrics metrics){
	if(enable_throughput){
		metrics.stop_throughput_clock = current_time_usecs();
	}
	if(enable_upl){
		//UPL metrics
		UPL_print_default_metrics();
		long long r_cache = UPL_get_cache_miss(metrics.fd_cache);
		if(r_cache < 0){
			std::cout << "Error when UPL_get_cache_miss(...)" << std::endl;
		}
		std::cout << "UPLib -> Total cache-miss(KB): " << r_cache << std::endl;

		if(UPL_finalize_count_rapl(metrics.rapl_fd) == 0){
			std::cout << "Error when UPL_finalize_count_rapl(...)" << std::endl;
		}
		std::cout << std::endl;
		delete metrics.rapl_fd;
	}

	if(print_latency){
		print_average_latency();
	}
	if(enable_throughput){
		print_throughput(metrics);
	}
	if(latency_to_file){
		write_latency(out_file_path("log") + "_latency.dat");
	}
	if (enable_monitoring){
		FILE *monitor_file;
		std::string file_name = (out_file_path("log") + "_monitoring.dat");

		monitor_file = fopen(file_name.c_str(), "w");
		fprintf(monitor_file, "Timestamp CPU_usage Mem_usage Throughput Latency\n");
	    for(unsigned int i = 0; i < monitor_vector.size(); i++){
			fprintf(monitor_file, "%.4f %.4f %ld %.4f %.4f\n", monitor_vector[i].timestamp, monitor_vector[i].cpu_usage, monitor_vector[i].mem_usage, monitor_vector[i].throughput, monitor_vector[i].latency_item);
	  	}
	  	fclose(monitor_file);
	}
}

void print_throughput(data_metrics metrics){
		printf("------------------ THROUGHPUT -----------------\n\n");
		printf("        Execution time = %f sec.\n", (metrics.stop_throughput_clock - metrics.start_throughput_clock) / 1000000.0);
		printf("       Itens processed = %lu \n", item_count);
		printf("      Itens per second = %f\n", (item_count * batch_size)/((metrics.stop_throughput_clock - metrics.start_throughput_clock) / 1000000.0));
		if(batch_size > 1){
			printf("     Batches processed = %lu \n", input_items);
			printf("    Batches per second = %f\n", item_count/((metrics.stop_throughput_clock - metrics.start_throughput_clock) / 1000000.0));
		}
		printf("\n-----------------------------------------------\n");
}

//Sum the individual latencies and print the average results
void print_average_latency(){
	std::vector<double> operator_aux;
	std::vector<std::string> operator_name_list = set_operators_name();
	double total = 0.0;

	for(unsigned int j = 0; j < latency_vector[0].local_latency.size(); j++)
		operator_aux.push_back(0.0); //initialize the vector to receive the sum of latencies

	for(unsigned int i = 0; i < latency_vector.size(); i++){
		for(unsigned int j = 0; j < latency_vector[i].local_latency.size(); j++) //get the latency of each operator 
			operator_aux[j] = operator_aux[j] + latency_vector[i].local_latency[j];
		total += latency_vector[i].local_total;
	}
	printf("-------------- AVERAGE LATENCY --------------\n\n");
	if(operator_name_list.size() != latency_vector[0].local_latency.size()){
		printf("Warning: the list of operator names does not match up the number of analyzed operators.\n");
		printf("The names will be changed to default names.\n\n");
		for(unsigned int j = 0; j < operator_aux.size(); j++)
			printf("\tOperator %d = %f\n", j, operator_aux[j]/latency_vector.size());
	} else {
		for(unsigned int j = 0; j < operator_aux.size(); j++)	
			printf("\tOperator %s = %f\n", operator_name_list[j].c_str(), operator_aux[j]/latency_vector.size());
	}
	printf("\n\tEnd-to-end latency = %f\n", total/latency_vector.size());
	printf("\n---------------------------------------------\n");
}


//Write each individual latency in a latency_result.txt file
void write_latency(std::string file_name){
	std::vector<std::string> operator_name_list = set_operators_name();
	FILE *latency_file;
	latency_file = fopen(file_name.c_str(), "w");

	//write the head of the file
	if(operator_name_list.size() != latency_vector[0].local_latency.size()){
		printf("Warning: the list of operator names does not match up the number of analyzed operators.\n");
		printf("The names will be changed to default names.\n\n");
		for(unsigned int i = 0; i < latency_vector[0].local_latency.size(); i++)
			fprintf(latency_file, "Operator%d ", i);
	} else {
		for(unsigned int i = 0; i < operator_name_list.size(); i++)
			fprintf(latency_file, "%s ", operator_name_list[i].c_str());
	}
	fprintf(latency_file, "\n");

	//write the results 
	for(unsigned int i = 0; i < latency_vector.size(); i++){
		for(unsigned int j = 0; j < latency_vector[i].local_latency.size(); j++)
			fprintf(latency_file, "%f ", latency_vector[i].local_latency[j]);
		fprintf(latency_file, "\n");
	}
	fclose(latency_file);
}

/*
std::string executable_name(){
	std::string name;
    std::ifstream("/proc/self/comm") >> name;
    
    return name;
}*/
