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

#include <spbench.hpp>
#include <upl.h>

namespace spb{

unsigned int nthreads = 1;

unsigned long SPBench::items_reading_frequency = 0; //usec precision
unsigned int SPBench::batch_size = 1;
bool SPBench::memory_source = false;
std::string SPBench::bench_path; //stores executable name from arg[0] in init_bench()
std::vector<std::string> SPBench::userArgs;
std::vector<std::string> SPBench::operator_name_list;

unsigned long Metrics::monitoring_time_interval = 250;
bool Metrics::latency = false;
bool Metrics::print_latency = false;
bool Metrics::latency_to_file = false;
bool Metrics::upl = false;
bool Metrics::throughput = false;
bool Metrics::monitoring = false;
Metrics::data_metrics Metrics::metrics;
unsigned long Metrics::batch_counter = 0; // batches processed
unsigned long Metrics::items_counter = 0;
unsigned long Metrics::items_at_sink_counter = 1;
volatile unsigned long Metrics::start_time_usec = 0;
volatile unsigned long Metrics::end_time_usec = 0;
volatile unsigned long Metrics::global_latency_acc = 0;
volatile unsigned long Metrics::global_current_latency = 0;
volatile unsigned long Metrics::execution_init_clock = 0;
volatile unsigned long Metrics::item_old_time = 0;
unsigned int Metrics::monitored_items_counter = 0;

std::vector<Metrics::latency_t> Metrics::latency_vector;
std::vector<Metrics::monitor_data> Metrics::monitor_vector;

// ns
std::vector<data_metrics> metrics_vec;

volatile unsigned long current_time_usecs() {
	struct timeval time;
	gettimeofday(&time, NULL);
	return time.tv_sec * 1000000 + time.tv_usec;
}

bool file_exists (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

std::string prepareOutFileAt(std::string out_path){
	if(!file_exists(out_path)){
		if (mkdir(out_path.c_str(), 0777) != 0){
			std::cerr << "Error :  " << strerror(errno) << std::endl;
		}
	}
	return (out_path + "/" + base_name(SPBench::getExecPath()));
}

std::string SPBench::getArg(unsigned int id){
	if(id >= userArgs.size()){
		std::cout << "Missing argument in position: " << id << std::endl;
		exit(0);
	}
	return userArgs[id];
}

void SPBench::setArg(std::string user_arg){
	userArgs.push_back(user_arg);
}

/* Item frequency control mechanism */
void SPBench::item_frequency_control(volatile unsigned long item_timestamp){
	while(1){
		if((current_time_usecs() - item_timestamp) >= get_items_reading_frequency()){
			break;
		}
	}
}

/* Item frequency control mechanism (nsources) */
void item_frequency_control(volatile unsigned long item_timestamp, volatile unsigned long sourceFrequency){
	while(1){
		if((current_time_usecs() - item_timestamp) >= sourceFrequency){
			break;
		}
	}
}

void Metrics::monitor_metrics(volatile unsigned long item_timestamp){
	monitored_items_counter++;
	monitor_data item_data;
	float elapsed_time = (current_time_usecs() - item_old_time)/1000.0;
	if(elapsed_time >= Metrics::get_monitoring_time_interval()){ //Time interval ms
		item_old_time = current_time_usecs();
		item_data.latency_item = ((current_time_usecs() - item_timestamp) / 1000.0);
		item_data.timestamp = (current_time_usecs() - execution_init_clock)/1000000.0;
		item_data.cpu_usage = UPL_get_proc_load_average_now(UPL_getProcID());
		item_data.mem_usage = UPL_getProcMemUsage();
		item_data.throughput = monitored_items_counter / item_data.timestamp;
		item_data.frequency = SPBench::get_items_reading_frequency();
		monitor_vector.push_back(item_data);
		//printf("%.4f %.4f %ld %.4f %.4f\n", item_data.timestamp, item_data.cpu_usage, item_data.mem_usage, item_data.throughput, item_data.latency_item);
	}
}

void monitor_metrics(volatile unsigned long item_timestamp, unsigned long int sourceId){
	metrics_vec[sourceId].monitored_items_counter++;
	monitor_data item_data;
	float elapsed_time = (current_time_usecs() - metrics_vec[sourceId].last_measured_time)/1000.0;
	if(elapsed_time >= Metrics::get_monitoring_time_interval()){ //Time interval ms
		metrics_vec[sourceId].last_measured_time = current_time_usecs();
		item_data.latency_item = ((current_time_usecs() - item_timestamp) / 1000.0);
		item_data.timestamp = (current_time_usecs() - metrics_vec[sourceId].start_throughput_clock)/1000000.0;
		item_data.cpu_usage = UPL_get_proc_load_average_now(UPL_getProcID());
		item_data.mem_usage = UPL_getProcMemUsage();
		item_data.throughput = metrics_vec[sourceId].monitored_items_counter / item_data.timestamp;
		//item_data.frequency = items_reading_frequency;
		metrics_vec[sourceId].monitor_vector.push_back(item_data);
		//printf("%.4f %.4f %ld %.4f %.4f\n", item_data.timestamp, item_data.cpu_usage, item_data.mem_usage, item_data.throughput, item_data.latency_item);
	}
}

void Metrics::init(){

	execution_init_clock = current_time_usecs();
	
	if(upl_is_enabled()){
		if(UPL_init_cache_miss_monitoring(&metrics.fd_cache) == 0){
			std::cout << "Error when UPL_init_cache_miss_monitoring(...)" << std::endl;
		}

		metrics.rapl_fd = new int[4];
		if(UPL_init_count_rapl(metrics.rapl_fd) == 0){
			std::cout << "Error when UPL_init_count_rapl(...)" << std::endl;
		}
	}
	if(throughput_is_enabled()){
		metrics.start_throughput_clock = current_time_usecs();
	}
	item_old_time = current_time_usecs();
	//return metrics;
}

data_metrics init_metrics(){

	//execution_init_clock = current_time_usecs();
	data_metrics metrics;
	if(Metrics::upl_is_enabled()){
		if(UPL_init_cache_miss_monitoring(&metrics.fd_cache) == 0){
			std::cout << "Error when UPL_init_cache_miss_monitoring(...)" << std::endl;
		}

		metrics.rapl_fd = new int[4];
		if(UPL_init_count_rapl(metrics.rapl_fd) == 0){
			std::cout << "Error when UPL_init_count_rapl(...)" << std::endl;
		}
	}
	if(Metrics::throughput_is_enabled()){
		metrics.start_throughput_clock = current_time_usecs();
	}
	//last_measured_time = current_time_usecs();
	return metrics;
}

void Metrics::stop(){

	if(Metrics::items_counter < 1){
		std::cout << "Error: your application processed zero items." << std::endl;
		return;
	}

	if(throughput_is_enabled()){
		metrics.stop_throughput_clock = current_time_usecs();
	}
	if(upl_is_enabled()){
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

	if(print_latency_is_enabled()){
		print_average_latency();
	}
	if(throughput_is_enabled()){
		print_throughput(metrics);
	}
	if(latency_to_file_is_enabled()){
		write_latency(prepareOutFileAt("log") + "_latency.dat");
	}
	if (monitoring_is_enabled()){
		FILE *monitor_file;
		std::string file_name = (prepareOutFileAt("log") + "_monitoring.dat");

		monitor_file = fopen(file_name.c_str(), "w");
		fprintf(monitor_file, "Timestamp CPU_usage Mem_usage Throughput Latency\n");
	    for(unsigned int i = 0; i < monitor_vector.size(); i++){
			fprintf(monitor_file, "%.4f %.4f %ld %.4f %.4f\n", monitor_vector[i].timestamp, monitor_vector[i].cpu_usage, monitor_vector[i].mem_usage, monitor_vector[i].throughput, monitor_vector[i].latency_item);
	  	}
	  	fclose(monitor_file);
	}
}

void compute_metrics(){

	for (auto & element : metrics_vec){
		if(element.global_item_counter < 1){
			std::cout << " Error: your application processed zero items." << std::endl;
			return;
		}
		
		if(Metrics::print_latency_is_enabled() || Metrics::throughput_is_enabled()){
			printf("\n===============================================\n");
			std::cout << " -> Resulting metrics for source: " << element.source_name << std::endl;
			printf(  "===============================================\n");
		}

		//if(Metrics::throughput_is_enabled()){
		//	element.stop_throughput_clock = current_time_usecs();
		//}
		if(Metrics::upl_is_enabled()){
			//UPL metrics
			UPL_print_default_metrics();
			long long r_cache = UPL_get_cache_miss(element.fd_cache);
			if(r_cache < 0){
				std::cout << "Error when UPL_get_cache_miss(...)" << std::endl;
			}
			std::cout << "UPLib . Total cache-miss(KB): " << r_cache << std::endl;

			if(UPL_finalize_count_rapl(element.rapl_fd) == 0){
				std::cout << "Error when UPL_finalize_count_rapl(...)" << std::endl;
			}
			std::cout << std::endl;
			delete element.rapl_fd;
		}

		if(Metrics::print_latency_is_enabled()){
			print_average_latency(element);
		}
		if(Metrics::throughput_is_enabled()){
			print_throughput(element);
		}
		if(Metrics::print_latency_is_enabled() || Metrics::throughput_is_enabled())
			printf("-----------------------------------------------\n");

		if(Metrics::latency_to_file_is_enabled()){
			//write_latency(out_file_path("log") + "_latency.dat");
			write_latency(element);
		}
		if (Metrics::monitoring_is_enabled()){
			FILE *monitor_file;
			std::string file_name = (prepareOutFileAt("log") + "_" + element.source_name + "_monitoring.dat");

			monitor_file = fopen(file_name.c_str(), "w");
			fprintf(monitor_file, "Timestamp CPU_usage Mem_usage Throughput Latency\n");
			for(unsigned int i = 0; i < element.monitor_vector.size(); i++){
				fprintf(monitor_file, "%.4f %.4f %ld %.4f %.4f\n", 
					element.monitor_vector[i].timestamp, 
					element.monitor_vector[i].cpu_usage, 
					element.monitor_vector[i].mem_usage, 
					element.monitor_vector[i].throughput, 
					element.monitor_vector[i].latency_item
					//element.monitor_vector[i].frequency
				);
			}
			fclose(monitor_file);
		}
	}
}

void Metrics::print_throughput(data_metrics metrics){
		printf("------------------ THROUGHPUT -----------------\n\n");
		printf("\tExecution time (sec) = %f\n\n", (metrics.stop_throughput_clock - metrics.start_throughput_clock) / 1000000.0);
		printf("\tItens processed = %lu\n", items_counter);
		printf("\tItens-per-second = %f\n\n", (items_counter * SPBench::get_batch_size())/((metrics.stop_throughput_clock - metrics.start_throughput_clock) / 1000000.0));
		if(items_counter != batch_counter){
			printf("\tBatches processed = %lu\n", batch_counter);
			printf("\tBatches-per-second = %f\n", batch_counter/((metrics.stop_throughput_clock - metrics.start_throughput_clock) / 1000000.0));
		}
		printf("\n-----------------------------------------------\n");
}

//Sum the individual latencies and print the average results
void Metrics::print_average_latency(){
	std::vector<double> operator_aux;
	//std::vector<std::string> operator_name_list = set_operators_name();
	double total = 0.0;

	if(latency_vector.size() < 1){
		std::cout << " Error: your application processed zero items." << std::endl;
		return;
	}

	for(unsigned int j = 0; j < latency_vector[0].local_latency.size(); j++)
		operator_aux.push_back(0.0); //initialize the vector to receive the sum of latencies

	for(unsigned int i = 0; i < latency_vector.size(); i++){
		for(unsigned int j = 0; j < latency_vector[i].local_latency.size(); j++) //get the latency of each operator 
			operator_aux[j] = operator_aux[j] + (latency_vector[i].local_latency[j]/1000.0);
		total += latency_vector[i].local_total/1000.0;
	}
	printf("\n--------------- AVERAGE LATENCY ---------------\n\n");
	if(SPBench::get_operator_name_list().size() != latency_vector[0].local_latency.size()){
		printf("Warning: the list of operator names does not match up the number of analyzed operators.\n");
		printf("The names will be changed to default names.\n\n");
		for(unsigned int j = 0; j < operator_aux.size(); j++)
			printf("\tOperator %d = %f\n", j, (operator_aux[j]/latency_vector.size()));
	} else {
		for(unsigned int j = 0; j < operator_aux.size(); j++)	
			printf("\tOperator %s = %f\n", SPBench::get_operator_name_list()[j].c_str(), (operator_aux[j]/latency_vector.size()));
	}
	printf("\n\tTotal-latency (ms) = %f\n", total/latency_vector.size());
	printf("\n-----------------------------------------------\n");
}

//Write each individual latency in a latency_result.txt file
void Metrics::write_latency(std::string file_name){
	//std::vector<std::string> operator_name_list = set_operators_name();
	FILE *latency_file;
	latency_file = fopen(file_name.c_str(), "w");

	//write the head of the file
	if(SPBench::get_operator_name_list().size() != latency_vector[0].local_latency.size()){
		printf("Warning: the list of operator names does not match up the number of analyzed operators.\n");
		printf("The names will be changed to default names.\n\n");
		for(unsigned int i = 0; i < latency_vector[0].local_latency.size(); i++)
			fprintf(latency_file, "Operator%d ", i);
	} else {
		for(unsigned int i = 0; i < SPBench::get_operator_name_list().size(); i++)
			fprintf(latency_file, "%s ", SPBench::get_operator_name_list()[i].c_str());
	}
	fprintf(latency_file, "\n");

	//write the results 
	for(unsigned int i = 0; i < latency_vector.size(); i++){
		for(unsigned int j = 0; j < latency_vector[i].local_latency.size(); j++)
			fprintf(latency_file, "%.4f ", (latency_vector[i].local_latency[j])/1000.0);
		fprintf(latency_file, "\n");
	}
	fclose(latency_file);
}

//Sum the individual latencies and print the average results
void print_average_latency(data_metrics metrics){
	std::vector<double> operator_aux;
	//std::vector<std::string> operator_name_list = set_operators_name();
	double total = 0.0;

	for(unsigned int j = 0; j < metrics.latency_vector_ns[0].local_latency.size(); j++)
		operator_aux.push_back(0.0); //initialize the vector to receive the sum of latencies

	for(unsigned int i = 0; i < metrics.latency_vector_ns.size(); i++){
		for(unsigned int j = 0; j < metrics.latency_vector_ns[i].local_latency.size(); j++) //get the latency of each operator 
			operator_aux[j] = operator_aux[j] + (metrics.latency_vector_ns[i].local_latency[j] / 1000.0);
		total += metrics.latency_vector_ns[i].local_total / 1000.0;
	}
	printf("-------------- AVERAGE LATENCIES --------------\n\n");
	if(SPBench::get_operator_name_list().size() != metrics.latency_vector_ns[0].local_latency.size()){
		printf("Warning: the list of operator names does not match up the number of analyzed operators.\n");
		printf("The names will be changed to default names.\n\n");
		for(unsigned int j = 0; j < operator_aux.size(); j++)
			printf("\t Operator %d = %f\n", j, (operator_aux[j]/metrics.latency_vector_ns.size()));
	} else {
		for(unsigned int j = 0; j < operator_aux.size(); j++)	
			printf("\t Operator %s = %f\n", SPBench::get_operator_name_list()[j].c_str(), (operator_aux[j]/metrics.latency_vector_ns.size()));
	}
	printf("\n\tEnd-to-end latency = %f (ms)\n\n", (total/metrics.latency_vector_ns.size()));
	return;
}

//Write each individual latency in a latency_result.txt file
void write_latency(data_metrics metrics){

	std::string file_name = prepareOutFileAt("log") + "_" + metrics.source_name + "_latency.dat";
	//std::vector<std::string> operator_name_list = set_operators_name();
	FILE *latency_file;
	latency_file = fopen(file_name.c_str(), "w");

	//write the head of the file
	if(SPBench::get_operator_name_list().size() != metrics.latency_vector_ns[0].local_latency.size()){
		printf("Warning: the list of operator names does not match up the number of analyzed operators.\n");
		printf("The names will be changed to default names.\n\n");
		for(unsigned int i = 0; i < metrics.latency_vector_ns[0].local_latency.size(); i++)
			fprintf(latency_file, "Operator%d ", i);
	} else {
		for(unsigned int i = 0; i < SPBench::get_operator_name_list().size(); i++)
			fprintf(latency_file, "%s ", SPBench::get_operator_name_list()[i].c_str());
	}
	fprintf(latency_file, "\n");

	//write the results 
	for(unsigned int i = 0; i < metrics.latency_vector_ns.size(); i++){
		for(unsigned int j = 0; j < metrics.latency_vector_ns[i].local_latency.size(); j++)
			fprintf(latency_file, "%.4f ", (metrics.latency_vector_ns[i].local_latency[j])/1000.0);
		fprintf(latency_file, "\n");
	}
	fclose(latency_file);
}

void print_throughput(data_metrics metrics){
		volatile unsigned long clock = metrics.stop_throughput_clock - metrics.start_throughput_clock;
		printf("------------------ THROUGHPUT -----------------\n\n");
		printf("\tExecution time = %.2f (seconds)\n\n", (clock / 1000000.0));
		printf("\tItems processed = %lu\n", metrics.global_item_counter);
		printf("\tItems-per-second = %.4f\n", metrics.global_item_counter/(clock / 1000000.0));
		if(metrics.global_item_counter != metrics.global_batch_counter){
			printf("\n\tBatches processed = %lu\n", metrics.global_batch_counter);
			printf("\tBatches-per-second = %.4f\n", metrics.global_batch_counter/(clock / 1000000.0));
		}
		printf("\n");
}

} //end of namespace spb

/*
std::string executable_name(){
	std::string name;
    std::ifstream("/proc/self/comm") >> name;
    
    return name;
}*/
/*
unsigned int freq_steps = 20;
unsigned int load = 0;
unsigned int x = 0;
unsigned int freq_step = 0;
unsigned int seil = 0;

//1 = asc, 2 = desc, 3 = wave, 4 = binary
unsigned int mode = 0;
unsigned int wave_resolution = 4;
bool cicled = true;

inline void PDP_freq_control(){

	mode = stoi(userArgs[0]);
	load = stoi(userArgs[1]);
	seil = stoi(userArgs[2]);
	x = load/freq_steps;

	if(spb::items_counter > (x*freq_step)){
		switch(mode){
			case 1:
				if(freq_step == 0){
					spb::items_reading_frequency = 0;
				} else {

					spb::items_reading_frequency = spb::items_reading_frequency + (seil/freq_steps);
				}
				break;
			case 2:
				if(freq_step == 0){
					spb::items_reading_frequency = seil;
				} else {
					spb::items_reading_frequency = spb::items_reading_frequency - (seil/freq_steps);
				}
				break;
			case 3:
				if(freq_step == 0){
						spb::items_reading_frequency = 0;
				} else {
					if((spb::items_reading_frequency < seil) && cicled){
						spb::items_reading_frequency = spb::items_reading_frequency + (seil/wave_resolution);
					} else {
						spb::items_reading_frequency = spb::items_reading_frequency - (seil/wave_resolution);
						cicled = false;
						if(spb::items_reading_frequency == 0)
							cicled = true;
					}
				}
				break;
			case 4:
				if(freq_step == 0){
					spb::items_reading_frequency = 0;
				} else {
					if(spb::items_reading_frequency == 0)
						spb::items_reading_frequency = seil;
					else
						spb::items_reading_frequency = 0;
				}
				break;
			default:
				//printf("No mode selected. Exit!\n");
				break;
		}
		std::cout << spb::items_reading_frequency << std::endl;
		freq_step++;
	}
}*/

