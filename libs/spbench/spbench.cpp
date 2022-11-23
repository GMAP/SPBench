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

#if !defined(NO_UPL)
	#include <upl.h>
#endif

namespace spb{
unsigned int nthreads = 1;

std::atomic<bool> execution_done(false);

float SPBench::items_reading_frequency = -1; // if <= 0.0, then ignore frequency control
//float SPBench::item_waiting_time = 0;
int SPBench::batch_size = 1;
float SPBench::batch_interval = 0.0;
frequencyPattern_t SPBench::freq_patt;

bool SPBench::memory_source = false;
std::string SPBench::bench_path; //stores executable name from arg[0] in init_bench()
std::vector<std::string> SPBench::userArgs;
std::vector<std::string> SPBench::operator_name_list;
unsigned long SPBench::pattern_cycle_start_time = 0;
int SPBench::number_of_operators = 0;

void frequency_pattern(double elapsed_time);

long Metrics::monitoring_time_interval = 250;
bool Metrics::latency = false;
bool Metrics::print_latency = false;
bool Metrics::latency_to_file = false;
bool Metrics::upl = false;
bool Metrics::throughput = false;
bool Metrics::monitoring = false;
bool Metrics::monitoring_thread = false;
Metrics::data_metrics Metrics::metrics;
long Metrics::batch_counter = 0; // batches processed
long Metrics::items_counter = 0;
long Metrics::items_at_sink_counter = 0;
long Metrics::batches_at_sink_counter = 0;
long Metrics::global_latency_acc = 0;
long Metrics::execution_init_clock = 0;
long Metrics::item_old_time = 0;

std::thread Metrics::monitor_thread;

int SuperSource::sourceObjCounter = 0;

std::vector<Metrics::item_metrics_data> Metrics::latency_vector;
std::vector<Metrics::monitor_data> Metrics::monitor_vector;
std::vector<unsigned long> Metrics::timestamps_vec;

// ns
std::vector<data_metrics> metrics_vec;

/**
 * @return current time using gettimeofday()
 */
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

/**
 * Print current date and time.
 *
 * It prints the current date and time as: YYYY-MM-DD HH-MM-SS
 *
 * @return nothing
 */
void print_date_time(){

	auto current_clock = std::chrono::system_clock::now();

	std::time_t end_time = std::chrono::system_clock::to_time_t(current_clock);

	std::tm* calendar_formatted = std::gmtime(&end_time);

	std::cout << " " << calendar_formatted->tm_year+1900 << "-" << calendar_formatted->tm_mon+1 << "-" 
				<< calendar_formatted->tm_mday << " " <<  calendar_formatted->tm_hour << ":" 
				<< calendar_formatted->tm_min << ":" <<  calendar_formatted->tm_sec << std::endl;
}

/**
 * Get user argument.
 *
 * It receives an integer value containing the users's custom 
 * argument position (first is at 0) and returns the respective argument
 *
 * @param id position of the user argument.
 * @return user argument on position id.
 */
std::string SPBench::getArg(int id){
	if(id >= userArgs.size()){
		std::cout << "Missing argument in position: " << id << std::endl;
		exit(0);
	}
	return userArgs[id];
}

/**
 * Get new operator ID.
 *
 * It computes an id for an operator.
 *
 * @param nothing
 * @return Integer ID.
 */
int SPBench::getNewOpId(){
	number_of_operators++;
	return number_of_operators - 1;
}

/**
 * Set user argument.
 *
 * It receives a string containing an custom argument given 
 * by the user and add it to the vector of users' arguments.
 *
 * @param user_arg String containing the user argument.
 * @return nothing.
 */
void SPBench::setArg(std::string user_arg){
	userArgs.push_back(user_arg);
}

/**
 * Split string.
 *
 * It receives a string and a char delimiter and
 * splits this string into a vector
 *
 * @param in_string input string.
 * @return a string vector containing the slices of the input string.
 */
std::vector<std::string> split_string(const std::string &in_string, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (in_string);
    std::string item;
    while (getline (ss, item, delim)) {
        result.push_back (item);
    }
    return result;
}

/**
 * Parser the input string for frequency pattern.
 *
 * It receives a string in the format <pattern,period,min,max,spike>
 * and sets up the respective pattern for the frequency.
 *
 * @param pattern String containing the input pattern.
 * @return nothing.
 */
void input_freq_pattern_parser(std::string pattern){
	float freq_period = 0.0;
	float freq_min = 0.0;
	float freq_max = 0.0;
	float freq_spike = 10;

	try{
		if((split_string(pattern, ',').size() < 4) || (split_string(pattern, ',').size() > 5)) {
			throw std::invalid_argument("\n ARGUMENT ERROR (-p <frequency_pattern>) --> Invalid argument. Required: -p <type,period,min,max,spike> (e.g. -p wave,10,5.2,80,15) (Obs: spike value is optional, it only aplies for the spike pattern)!\n");
		}
		try{
			freq_period = std::stof(split_string(pattern, ',')[1]);
			freq_min = std::stof(split_string(pattern, ',')[2]);
			freq_max = std::stof(split_string(pattern, ',')[3]);
			if (split_string(pattern, ',').size() == 5){
				freq_spike = std::stof(split_string(pattern, ',')[4]);
			}
		} catch (...) {
			throw std::invalid_argument("\n ARGUMENT ERROR (-p <frequency_pattern>) --> Invalid argument value. Required: -p <type(string),period(float),min(float),max(float),spike(0-100 float)> (e.g. -p wave,10,5.2,80,15)!\n");
		}
		SPBench::setFrequencyPattern(split_string(pattern, ',')[0], freq_period, freq_min, freq_max, freq_spike);
	} catch (...) {
		throw;
	}
}

/**
 * Set frequency pattern
 * 
 * It presets up a frequency pattern.
 *
 * @param freq_pattern: it is the name of a frequency pattern, e.g. wave, increasing, etc.
 * @param freq_period: it is the time period of a cycle in the pattern
 * @param freq_low: it is the minimum possible frequency
 * @param freq_high: it is the maxiumum possible frequency
 * @param freq_spike: it is the spike size as percentage of the period (0-100) (optional) (default is 10%)
 *
 * @return nothing.
 */
void SPBench::setFrequencyPattern(std::string freq_pattern, float freq_period, float freq_low, float freq_high, float freq_spike){
	if(freq_pattern == "none") return;
		
	try{
		if(freq_period < 0) throw std::invalid_argument("\n ERROR in setFrequencyPattern() --> Period must be higher than zero. Given value: " + std::to_string(freq_period) + "!\n");
		if(freq_low < 0) throw std::invalid_argument("\n ERROR in setFrequencyPattern() --> Minimum frequency must be higher than zero. Given value: " + std::to_string(freq_low) + "!\n");
		if(freq_high < 0) throw std::invalid_argument("\n ERROR in setFrequencyPattern() --> maxiumum frequency must be higher than zero. Given value: " + std::to_string(freq_high) + "!\n");
		if(freq_low > freq_high) throw std::invalid_argument("\n ERROR in setFrequencyPattern() --> Minimum frequency must be lower than maximum. Given values:  min = " + std::to_string(freq_low) + ", max = " + std::to_string(freq_high) + "!\n");
		if(freq_pattern == "wave" || freq_pattern == "binary" || freq_pattern == "spike" || freq_pattern == "increasing" || freq_pattern == "decreasing"){
			if(freq_pattern == "spike"){
				if((freq_spike < 0) || (freq_spike > 100)){
					throw std::invalid_argument("\n ERROR in setFrequencyPattern() --> Spike size must be a value between 0 and 100 (percentage of the period). Given value: " + std::to_string(freq_spike) + "!\n");
				}
				freq_patt.spikeInterval = freq_patt.period * (freq_spike / 100.0);
			}
			freq_patt.pattern = freq_pattern;
			freq_patt.period = freq_period;
			freq_patt.low = freq_low;
			freq_patt.high = freq_high;

			freq_patt.range_freq = freq_high - freq_low;
			freq_patt.amplitude = freq_patt.range_freq / 2;
			freq_patt.off_set = freq_low + freq_patt.amplitude; // vertical shift
			freq_patt.step = freq_patt.range_freq / freq_period;
			freq_patt.wavelength = 1/freq_patt.period;
			freq_patt.wave_preset = 2 * M_PI * freq_patt.wavelength; // we do this here because it can be computed only once
			
			if(freq_pattern == "wave")
				setFrequency(freq_patt.off_set);
			if(freq_pattern == "increasing" || freq_pattern == "binary" || freq_pattern == "spike")
				setFrequency(freq_low);
			if(freq_pattern == "decreasing")
				setFrequency(freq_high);
		} else {
			throw std::invalid_argument("\n ERROR in setFrequencyPattern() --> Invalid pattern: " + freq_pattern + "!\n");
		}
	} catch (...) {
		throw;
	}
}

/**
 * Item frequency control
 * 
 * It computes a waiting time based on a desired frequency and wait.
 *
 * @param last_source_item_timestamp: the timestamp of the last item that left the source.
 *
 * @return nothing.
 */
void SPBench::item_frequency_control(unsigned long last_source_item_timestamp) { //receives the target rate
	// if no value was given for items_reading_frequency, then ignore frequency control
	if(items_reading_frequency <= 0.0) 
		return;
	
	// Run the pattern computation (if set one) to set the correct items_reading_frequency
	SPBench::frequency_pattern();

	// The time the source toke to process and send the last item
	float last_source_item_processing_time = (current_time_usecs() - last_source_item_timestamp);

	// Expected sleep time considering only the target frequency, 
	// Divided by 1000000 in order to estimate the time interval in microseconds to wait before generating the next item. 
	float expected_waiting_time = (1000000/items_reading_frequency);

	// Some of the expected waiting time has already spent by the source to compute last item
	// So cut this spent time off the expected time for it to be more precise
	float actual_waiting_time = expected_waiting_time - last_source_item_processing_time;

	//std::cout << last_source_item_processing_time << " " << expected_waiting_time << " " << actual_waiting_time << std::endl;
	// If the source spent more time than the expected waiting time, do not sleep
	if(actual_waiting_time > 0)
		usleep(actual_waiting_time); // 1000: milliseconds to microseconds
}

/**
 * Item frequency control for nsources benchmarks
 * 
 * It computes a waiting time based on a desired frequency and wait.
 *
 * @param last_source_item_timestamp: the timestamp of the last item that left the source.
 * @param items_reading_frequency: the target frequency of the respective source.
 *
 * @return nothing.
 */
void item_frequency_control(unsigned long last_source_item_timestamp, float items_reading_frequency) { //receives the target rate
	// if no value was given for items_reading_frequency, then ignore frequency control
	if(items_reading_frequency <= 0.0) 
		return;
	
	// Run the pattern computation (if set one) to set the correct items_reading_frequency
	SPBench::frequency_pattern();

	// The time the source toke to process and send the last item
	float last_source_item_processing_time = (current_time_usecs() - last_source_item_timestamp);

	// Expected sleep time considering only the target frequency, 
	// Divided by 1000000 in order to estimate the time interval in microseconds to wait before generating the next item. 
	float expected_waiting_time = (1000000/items_reading_frequency);

	// Some of the expected waiting time has already spent by the source to compute last item
	// So cut this spent time off the expected time for it to be more precise
	float actual_waiting_time = expected_waiting_time - last_source_item_processing_time;

	//std::cout << last_source_item_processing_time << " " << expected_waiting_time << " " << actual_waiting_time << std::endl;
	// If the source spent more time than the expected waiting time, do not sleep
	if(actual_waiting_time > 0)
		usleep(actual_waiting_time); // 1000: milliseconds to microseconds
}

long spike_cycle_start_time = 0;
bool max_state = false; // for binary freq. pattern
/**
 * Frequency pattern
 * 
 * It computes a target frequency based on a pattern given by users.
 *
 * @return nothing.
 */
void SPBench::frequency_pattern(){

	if(freq_patt.pattern == "none") return;

	double elapsed_time = (current_time_usecs() - Metrics::execution_init_clock) / 1000000.0;

	float frequency;
    if(freq_patt.pattern == "wave"){ //sin formula: AMPLITUDE * sin(2 * M_PI * wavelength * time + off_set) + OFFSET;
        items_reading_frequency = (freq_patt.amplitude * sin(freq_patt.wave_preset * elapsed_time)) + freq_patt.off_set;
	} else if(freq_patt.pattern == "spike"){

		float pattern_cycle_elapsed_time = (current_time_usecs() - pattern_cycle_start_time) / 1000000.0;
		 
        if(pattern_cycle_elapsed_time > freq_patt.period - freq_patt.spikeInterval){
			float step = freq_patt.range_freq / freq_patt.spikeInterval;

			items_reading_frequency = freq_patt.low + ((pattern_cycle_elapsed_time - (freq_patt.period - freq_patt.spikeInterval)) * step);

			if(pattern_cycle_elapsed_time > freq_patt.period){
				pattern_cycle_start_time = current_time_usecs();
				items_reading_frequency = freq_patt.low;
			}
		}
    } else if(freq_patt.pattern == "binary"){       		
		float half_period_elapsed_time = (current_time_usecs() - pattern_cycle_start_time) / 1000000.0;
		if(half_period_elapsed_time > freq_patt.period / 2){
			pattern_cycle_start_time = current_time_usecs();
			if(!max_state){
				items_reading_frequency = freq_patt.high;
				max_state = true;
			} else {
				items_reading_frequency = freq_patt.low;
				max_state = false;
			}
		}
    } else if(freq_patt.pattern == "increasing"){
        float item_elapsed_time = (current_time_usecs() - pattern_cycle_start_time) / 1000000.0;
        if(items_reading_frequency < freq_patt.high) {
			// multiply the step it should increase per second by the elapsed time in seconds since the last step
            items_reading_frequency += item_elapsed_time * freq_patt.step;
		} else {
            items_reading_frequency = freq_patt.high;
		}
        pattern_cycle_start_time = current_time_usecs();
    } else if(freq_patt.pattern == "decreasing"){
        float item_elapsed_time = (current_time_usecs() - pattern_cycle_start_time) / 1000000.0;
        if(items_reading_frequency > freq_patt.low){
			// multiply the step it should decrease per second by the elapsed time in seconds since the last step
            items_reading_frequency -= item_elapsed_time * freq_patt.step;
		} else {
            items_reading_frequency = freq_patt.low;
		}
        pattern_cycle_start_time = current_time_usecs();
    }
}

/**
 * CPU usage
 * 
 * @return average load of the CPUs.
 */
float Metrics::getCPUUsage(){
	#if !defined(NO_UPL)
		return UPL_get_proc_load_average_now(UPL_getProcID());
	#endif
	return 0.0;
}

/**
 * Memory usage
 * 
 * @return memory usage of the respective process
 */
float Metrics::getMemoryUsage(){
	#if !defined(NO_UPL)
		return UPL_getProcMemUsage();
	#endif
	return 0.0;
}

/**
 * Instantaneous throughput
 * 
 * It computes the average throughput over a short time window.
 * 
 * @param time_window_in_seconds: the time window in seconds.
 * @return nothing.
 */
float Metrics::getInstantThroughput(float time_window_in_seconds){

	// if a single item was processed, than the throughput in this time window = 1
	if(latency_vector.size() < 2) return 1;

	long last_element_index = latency_vector.size()-1;
		
	float window_computed_time_sec = 0.0;
	long index = last_element_index;
	long total_items = latency_vector[last_element_index].batch_size; // count the number of items in the last batch
	while(1) {
		index--;
		// Compute the elapsed time from the last item to the item at index
		window_computed_time_sec = (latency_vector[last_element_index].item_sink_timestamp - latency_vector[index].item_sink_timestamp) / 1000000.0;

		//if the elapsed time between if the pair of items above is hihger than the time window, then break
		if(window_computed_time_sec >= time_window_in_seconds) break;

		total_items += latency_vector[index].batch_size; // count the number of items in the remaining batches

		if(index <= 0) break;
	}

	if(window_computed_time_sec <= 0.0) return 0.0;
	
	return total_items / window_computed_time_sec;
}

/**
 * Instantaneous throughput for n-sources benchmarks
 * 
 * It computes the average throughput over a short time window.
 * 
 * @param time_window_in_seconds: the time window in seconds.
 * @param sourceId: the index of the respective source.
 * @return nothing.
 */
float instantThroughput(float time_window_in_seconds, long sourceId){

	// if a single item was processed, than the throughput in this time window = 1
	if(metrics_vec[sourceId].latency_vector_ns.size() < 2) return 1;

	long last_element_index = metrics_vec[sourceId].latency_vector_ns.size()-1;
		
	float window_computed_time_sec = 0.0;
	long index = last_element_index;
	long total_items = metrics_vec[sourceId].latency_vector_ns[last_element_index].batch_size; // count the number of items in the last batch
	while(1) {
		index--;
		// Compute the elapsed time from the last item to the item at index
		window_computed_time_sec = (metrics_vec[sourceId].latency_vector_ns[last_element_index].item_sink_timestamp - metrics_vec[sourceId].latency_vector_ns[index].item_sink_timestamp) / 1000000.0;

		//if the elapsed time between if the pair of items above is hihger than the time window, then break
		if(window_computed_time_sec >= time_window_in_seconds) break;

		total_items += metrics_vec[sourceId].latency_vector_ns[index].batch_size; // count the number of items in the remaining batches

		if(index <= 0) break;
	
	}
	if(window_computed_time_sec <= 0.0) return 0.0;

	return total_items / window_computed_time_sec;
}

/**
 * Instantaneous latency
 * 
 * It computes the average latency over a short time window.
 * 
 * @param time_window_in_seconds: the time window in seconds.
 * @return nothing.
 */
float Metrics::getInstantLatency(float time_window_in_seconds){
	if(latency_vector.size() < 1){
		return 0.0;
	}
	long last_element_index = latency_vector.size()-1;
	long index = last_element_index;
	long number_of_items = 0;
	float inst_latency = latency_vector[last_element_index].total_latency;
	float window_computed_time_sec = 0.0;
	
	// if a single item was processed, than return its latency
	if (latency_vector.size() < 2) return inst_latency / 1000.0;
	
	// Sum latencies while computing window is not closed and there is elements to compute
	while(1) {
		index--;
		
		// Compute the elapsed time from the last item to the item at index
		window_computed_time_sec = (latency_vector[last_element_index].item_sink_timestamp - latency_vector[index].item_sink_timestamp) / 1000000.0;

		//if the elapsed time between if the pair of items above is hihger than the time window, then break
		if(window_computed_time_sec >= time_window_in_seconds){
			index++; // return index to the last valid position
			break;
		}
		// take the latency of the remaining batches
		inst_latency += latency_vector[index].total_latency;

		if(index <= 0) break;
	}
	if((latency_vector.size() - index) <= 0) return 0.0;

	//std::cout << (last_element_index - index) << " " << window_computed_time_sec  << " " << time_window_in_seconds << std::endl;
	return (inst_latency / (latency_vector.size() - index)) / 1000.0; //ms
}

/**
 * Instantaneous latency for n-sources benchmarks
 * 
 * It computes the average latency over a short time window.
 * 
 * @param time_window_in_seconds: the time window in seconds.
 * @param sourceId: the index of the respective source.
 * @return nothing.
 */
float instantLatency(float time_window_in_seconds, long sourceId){
	if(metrics_vec[sourceId].latency_vector_ns.size() < 1){
		return 0.0;
	}
	long last_element_index = metrics_vec[sourceId].latency_vector_ns.size()-1;
	long index = last_element_index;
	float inst_latency = metrics_vec[sourceId].latency_vector_ns[last_element_index].total_latency; // take the latency of the last batch
	float window_computed_time_sec = 0.0;
	
	// if a single item was processed, than return its latency
	if (metrics_vec[sourceId].latency_vector_ns.size() < 2) return inst_latency / 1000.0;
	
	// Sum latencies while computing window is not closed and there is elements to compute
	while(1) {
		index--;
		
		// Compute the elapsed time from the last item to the item at index
		window_computed_time_sec = (metrics_vec[sourceId].latency_vector_ns[last_element_index].item_sink_timestamp - metrics_vec[sourceId].latency_vector_ns[index].item_sink_timestamp) / 1000000.0;

		//if the elapsed time between if the pair of items above is hihger than the time window, then break
		if(window_computed_time_sec >= time_window_in_seconds){
			index++; // return index to the last valid position
			break;
		}

		// take the latency of the remaining batches
		inst_latency += metrics_vec[sourceId].latency_vector_ns[index].total_latency;

		if(index <= 0) break;
	}
	if((metrics_vec[sourceId].latency_vector_ns.size() - index) <= 0) return 0.0;

	//std::cout << (last_element_index - index) << " " << window_computed_time_sec  << " " << time_window_in_seconds << std::endl;
	return (inst_latency / (metrics_vec[sourceId].latency_vector_ns.size() - index)) / 1000.0; //ms
}


/**
 * Monitor metrics
 * 
 * It computes all monitoring metrics and stores it in a metrics vector
 * 
 * @return nothing.
 */
void Metrics::monitor_metrics(){
	monitor_data item_data;
	long current_time = current_time_usecs();
	float time_elapsed_from_last_measurement_ms = (current_time - item_old_time)/1000.0;
	if(time_elapsed_from_last_measurement_ms >= Metrics::get_monitoring_time_interval()){ //Time interval ms
		item_old_time = current_time;
		item_data.timestamp = (current_time - execution_init_clock)/1000000.0;
		item_data.cpu_usage = getCPUUsage();
		item_data.mem_usage = getMemoryUsage();
		item_data.average_throughput = getAverageThroughput();
		item_data.instant_throughput = getInstantThroughput(time_elapsed_from_last_measurement_ms / 1000);
		item_data.average_latency = getAverageLatency();
		item_data.instant_latency = getInstantLatency(time_elapsed_from_last_measurement_ms / 1000);
		item_data.frequency = SPBench::getFrequency() < 0.0 ? 0.0 : SPBench::getFrequency();
		item_data.batch_size = latency_vector.back().batch_size;
		monitor_vector.push_back(item_data);
		//printf("%.4f %.4f %ld %.4f %.4f\n", item_data.timestamp, item_data.cpu_usage, item_data.mem_usage, item_data.throughput, item_data.latency_item);
	}
}

/**
 * Monitor metrics (threaded version)
 * 
 * It computes all monitoring metrics and stores it in a metrics vector
 * It runs on an individual thread.
 * 
 * @return nothing.
 */
void Metrics::monitor_metrics_thread(){
	
	while(!execution_done){

		// Do not do anything while there is no data available
		if(latency_vector.size() < 1) continue;

		float time_elapsed_from_last_measurement_ms = (current_time_usecs() - item_old_time)/1000.0;
		
		std::this_thread::sleep_for(std::chrono::milliseconds(Metrics::get_monitoring_time_interval() - (long) time_elapsed_from_last_measurement_ms));// - (long) time_elapsed_from_last_measurement_ms));
	
		monitor_data item_data;
		item_old_time = current_time_usecs();
		item_data.instant_throughput = getInstantThroughput(time_elapsed_from_last_measurement_ms / 1000);
		item_data.instant_latency = getInstantLatency(time_elapsed_from_last_measurement_ms / 1000);

		item_data.timestamp = (current_time_usecs() - execution_init_clock)/1000000.0;
		item_data.cpu_usage = getCPUUsage();
		item_data.mem_usage = getMemoryUsage();
		item_data.average_throughput = getAverageThroughput();
		
		item_data.average_latency = getAverageLatency();
		item_data.frequency = SPBench::getFrequency() < 0.0 ? 0.0 : SPBench::getFrequency();
		item_data.batch_size = latency_vector.back().batch_size;
		
		monitor_vector.push_back(item_data);
		//printf("%.4f %.4f %ld %.4f %.4f\n", item_data.timestamp, item_data.cpu_usage, item_data.mem_usage, item_data.instant_throughput, item_data.instant_latency);
	}
	return;
}

/**
 * Monitor metrics for n-sources benchmarks
 * 
 * It computes all monitoring metrics and stores it the respective source's metrics vector
 * 
 * @param item_timestamp: the time stamp of the last arrived item.
 * @param sourceId: the index of the respective source.
 * @return nothing.
 */
void monitor_metrics(unsigned long item_timestamp, unsigned long int sourceId){
	monitor_data item_data;
	long current_time = current_time_usecs();
	float time_elapsed_from_last_measurement_ms = (current_time - metrics_vec[sourceId].last_measured_time)/1000.0;
	if(time_elapsed_from_last_measurement_ms >= Metrics::get_monitoring_time_interval()){ //Time interval ms
		metrics_vec[sourceId].last_measured_time = current_time;
		item_data.timestamp = (current_time - metrics_vec[sourceId].start_throughput_clock)/1000000.0;
		#if !defined(NO_UPL)
			item_data.cpu_usage = UPL_get_proc_load_average_now(UPL_getProcID());
			item_data.mem_usage = UPL_getProcMemUsage();
		#endif
		item_data.average_throughput = metrics_vec[sourceId].batches_at_sink_counter / item_data.timestamp;
		item_data.instant_throughput = instantThroughput((time_elapsed_from_last_measurement_ms / 1000), sourceId);
		item_data.average_latency = (metrics_vec[sourceId].global_latency_acc/metrics_vec[sourceId].batches_at_sink_counter) / 1000.0;
		item_data.instant_latency = instantLatency((time_elapsed_from_last_measurement_ms / 1000), sourceId);
		metrics_vec[sourceId].monitor_vector.push_back(item_data);
		/*printf("%.4f %.4f %ld %.4f %.4f %.4f %.4f\n", 
			item_data.timestamp, 
			item_data.cpu_usage, 
			item_data.mem_usage, 
			item_data.average_throughput, 
			item_data.instant_throughput,
			item_data.average_latency, 
			item_data.instant_latency
		);*/
	}
}

/**
 * Metrics initialization
 * 
 * It initializes the clocks and other parameters.
 * 
 * @return nothing.
 */
void Metrics::init(){

	#if !defined(NO_UPL)
		if(upl_is_enabled()){
			if(UPL_init_cache_miss_monitoring(&metrics.fd_cache) == 0){
				std::cout << "Error when UPL_init_cache_miss_monitoring(...)" << std::endl;
			}

			metrics.rapl_fd = new int[4];
			if(UPL_init_count_rapl(metrics.rapl_fd) == 0){
				std::cout << "Error when UPL_init_count_rapl(...)" << std::endl;
			}
		}
	#endif
	if(throughput_is_enabled()){
		metrics.start_throughput_clock = current_time_usecs();
	}
	
	SPBench::pattern_cycle_start_time = item_old_time = execution_init_clock = current_time_usecs();
}

/**
 * Metrics initialization for n-source benchmarks
 * 
 * It initializes the clocks and other parameters.
 * 
 * @return nothing.
 */
data_metrics init_metrics(){

	//execution_init_clock = current_time_usecs();
	data_metrics metrics;
	#if !defined(NO_UPL)
		if(Metrics::upl_is_enabled()){
			if(UPL_init_cache_miss_monitoring(&metrics.fd_cache) == 0){
				std::cout << "Error when UPL_init_cache_miss_monitoring(...)" << std::endl;
			}

			metrics.rapl_fd = new int[4];
			if(UPL_init_count_rapl(metrics.rapl_fd) == 0){
				std::cout << "Error when UPL_init_count_rapl(...)" << std::endl;
			}
		}
	#endif
	if(Metrics::throughput_is_enabled()){
		metrics.start_throughput_clock = current_time_usecs();
	}
	SPBench::pattern_cycle_start_time = current_time_usecs();

	return metrics;
}

/**
 * Stop metrics
 * 
 * It stops the clocks, computes and write the resulting metrics on the screen or on files. 
 * 
 * @return nothing.
 */
void Metrics::stop(){
	
	std::cout << " The execution ended on:"; 
	print_date_time();
	std::cout << std::endl;

	execution_done = true;

	if (monitor_thread.joinable()){
		monitor_thread.join();
	}
	
	if(Metrics::items_counter < 1){
		std::cout << "Error: your application processed zero items." << std::endl;
		return;
	}

	if(throughput_is_enabled()){
		metrics.stop_throughput_clock = current_time_usecs();
	}

	#if !defined(NO_UPL)
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
	#endif

	if(print_latency_is_enabled()){
		print_average_latency();
	}
	if(throughput_is_enabled()){
		print_throughput(metrics);
	}
	if(latency_to_file_is_enabled()){
		write_latency(prepareOutFileAt("log") + "_latency.dat");
	}
	if (monitoring_is_enabled() or monitoring_thread_is_enabled()){
		FILE *monitor_file;
		std::string file_name = (prepareOutFileAt("log") + "_monitoring_" + std::to_string(nthreads) + "nth.dat");

		monitor_file = fopen(file_name.c_str(), "w");
		fprintf(monitor_file, "Timestamp CPU_usage Mem_usage Avg_thr Inst_thr Avg_lat Inst_lat Tgt_freq Batch_size\n");
	    for(unsigned int i = 0; i < monitor_vector.size(); i++){
			fprintf(monitor_file, "%.4f %.4f %ld %.4f %.4f %.4f %.4f %.4f %u\n",
				monitor_vector[i].timestamp, 
				monitor_vector[i].cpu_usage, 
				monitor_vector[i].mem_usage, 
				monitor_vector[i].average_throughput,
				monitor_vector[i].instant_throughput,
				monitor_vector[i].average_latency, 
				monitor_vector[i].instant_latency, 
				monitor_vector[i].frequency,
				monitor_vector[i].batch_size
			);
	  	}
	  	fclose(monitor_file);
	}
}

/**
 * Compute metrics for n-source benchmarks
 * 
 * It computes and write the resulting metrics on the screen or on files for all the sources.
 * 
 * @return nothing.
 */
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

		#if !defined(NO_UPL)
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
		#endif

		if(Metrics::print_latency_is_enabled()){
			print_average_latency(element);
		}
		if(Metrics::throughput_is_enabled()){
			print_throughput(element);
		}
		if(Metrics::print_latency_is_enabled() || Metrics::throughput_is_enabled())
			printf("-----------------------------------------------\n");

		if(Metrics::latency_to_file_is_enabled()){
			write_latency(element);
		}
		if (Metrics::monitoring_is_enabled()){
			FILE *monitor_file;
			std::string file_name = (prepareOutFileAt("log") + "_" + element.source_name + "_monitoring.dat");

			monitor_file = fopen(file_name.c_str(), "w");
			fprintf(monitor_file, "Timestamp CPU_usage Mem_usage Avg_thr Inst_thr Avg_lat Inst_lat\n");
			for(unsigned int i = 0; i < element.monitor_vector.size(); i++){
				fprintf(monitor_file, "%.4f %.4f %ld %.4f %.4f %.4f %.4f\n",
					element.monitor_vector[i].timestamp, 
					element.monitor_vector[i].cpu_usage, 
					element.monitor_vector[i].mem_usage, 
					element.monitor_vector[i].average_throughput,
					element.monitor_vector[i].instant_throughput,
					element.monitor_vector[i].average_latency, 
					element.monitor_vector[i].instant_latency
				);
			}
			fclose(monitor_file);
		}
	}
}

/**
 * Print global average throughput
 * 
 * It prints throughput metrics at the end of the execution.
 * 
 * @return nothing.
 */
void Metrics::print_throughput(data_metrics metrics){
	unsigned long clock = metrics.stop_throughput_clock - metrics.start_throughput_clock;
	printf("------------------ THROUGHPUT -----------------\n\n");
	printf("\tExecution time (sec) = %f\n\n", clock / 1000000.0);
	printf("\tItems processed = %lu\n", items_at_sink_counter);
	printf("\tItems-per-second = %f\n\n", items_at_sink_counter/(clock / 1000000.0));
	if(items_at_sink_counter != batches_at_sink_counter){
		printf("\tBatches processed = %lu\n", batches_at_sink_counter);
		printf("\tBatches-per-second = %f\n", batches_at_sink_counter/(clock / 1000000.0));
	}
	printf("\n-----------------------------------------------\n");
}

/**
 * Print global average throughput for n-sources benchmarks
 * 
 * It prints throughput metrics at the end of the execution.
 * 
 * @param metrics data metrics of a specific source
 * @return nothing.
 */
void print_throughput(data_metrics metrics){
	unsigned long clock = metrics.stop_throughput_clock - metrics.start_throughput_clock;
	printf("------------------ THROUGHPUT -----------------\n\n");
	printf("\tExecution time (sec) = %f\n\n", (clock / 1000000.0));
	printf("\tItems processed = %lu\n", metrics.items_at_sink_counter);
	printf("\tItems-per-second = %f\n", metrics.items_at_sink_counter/(clock / 1000000.0));
	if(metrics.items_at_sink_counter != metrics.batches_at_sink_counter){
		printf("\n\tBatches processed = %lu\n", metrics.batches_at_sink_counter);
		printf("\tBatches-per-second = %f\n", metrics.batches_at_sink_counter/(clock / 1000000.0));
	}
	printf("\n");
}

/**
 * Print average latency
 * 
 * It prints global average latencies
 * 
 * @return nothing.
 */
void Metrics::print_average_latency(){

	if(latency_vector.size() < 1){
		std::cerr << " Error: your application processed zero items." << std::endl;
		return;
	}

	std::vector<double> operator_aux;
	//std::vector<std::string> operator_name_list = set_operators_name();
	double total = 0.0;

	for(unsigned int j = 0; j < latency_vector[0].local_latency.size(); j++)
		operator_aux.push_back(0.0); //initialize the vector to receive the sum of latencies

	float max_latency = 0.0;
	float min_latency = latency_vector[0].total_latency;
	float max_ts = 0.0, min_ts = 0.0;
	for(unsigned int i = 0; i < latency_vector.size(); i++){
		for(unsigned int j = 0; j < latency_vector[i].local_latency.size(); j++) //get the latency of each operator 
			operator_aux[j] = operator_aux[j] + (latency_vector[i].local_latency[j]/1000.0);
		total += latency_vector[i].total_latency;
		if(latency_vector[i].total_latency > max_latency){
			max_latency = latency_vector[i].total_latency;
			max_ts = (latency_vector[i].item_sink_timestamp - execution_init_clock);
		}
		if(latency_vector[i].total_latency < min_latency) {
			min_latency = latency_vector[i].total_latency;
			max_ts = (latency_vector[i].item_sink_timestamp - execution_init_clock);
		}
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
	printf("\n  End-to-end latency (ms) = %.3f\n", (total/latency_vector.size())/1000.0);
	printf("\n     Maximum latency (ms) = %.3f (at %.1f sec)\n", max_latency/1000.0, max_ts/1000000.0);
	printf("     Minimum latency (ms) = %.3f (at %.1f sec)\n", min_latency/1000.0, min_ts/1000000.0);
	printf("\n-----------------------------------------------\n");
}

/**
 * Print average latency
 * 
 * It prints global average latencies
 * 
 * @param metrics data metrics of a specific source
 * @return nothing.
 */
void print_average_latency(data_metrics metrics){

	if(metrics.latency_vector_ns.size() < 1){
		std::cerr << " Error: your application processed zero items." << std::endl;
		return;
	}

	std::vector<double> operator_aux;
	//std::vector<std::string> operator_name_list = set_operators_name();
	double total = 0.0;

	for(unsigned int j = 0; j < metrics.latency_vector_ns[0].local_latency.size(); j++)
		operator_aux.push_back(0.0); //initialize the vector to receive the sum of latencies

	float max_latency = 0.0;
	float min_latency = metrics.latency_vector_ns[0].total_latency;
	float max_ts = 0.0, min_ts = 0.0;
	for(unsigned int i = 0; i < metrics.latency_vector_ns.size(); i++){
		for(unsigned int j = 0; j < metrics.latency_vector_ns[i].local_latency.size(); j++) //get the latency of each operator 
			operator_aux[j] = operator_aux[j] + (metrics.latency_vector_ns[i].local_latency[j] / 1000.0);
		total += metrics.latency_vector_ns[i].total_latency;
		if(metrics.latency_vector_ns[i].total_latency > max_latency){
			max_latency = metrics.latency_vector_ns[i].total_latency;
			max_ts = (metrics.latency_vector_ns[i].item_sink_timestamp - metrics.latency_vector_ns[0].item_timestamp);
		}
		if(metrics.latency_vector_ns[i].total_latency < min_latency){
			min_latency = metrics.latency_vector_ns[i].total_latency;
			min_ts = (metrics.latency_vector_ns[i].item_sink_timestamp - metrics.latency_vector_ns[0].item_timestamp);
		}
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
	printf("\n  End-to-end latency (ms) = %f \n", (total/metrics.latency_vector_ns.size())/1000.0);
	printf("\n     Maximum latency (ms) = %.3f\n", max_latency/1000.0);
	printf("     Minimum latency (ms) = %.3f\n", min_latency/1000.0);
	printf("\n-----------------------------------------------\n");
	return;
}

/**
 * Write latency results
 * 
 * It computes and writes to a file the global average latency of each operator
 * 
 * @param file_name name of the output file
 * @return nothing.
 */
void Metrics::write_latency(std::string file_name){
	//std::vector<std::string> operator_name_list = set_operators_name();
	FILE *latency_file;
	latency_file = fopen(file_name.c_str(), "w");

	//write the head of the file
	if(SPBench::get_operator_name_list().size() != latency_vector[0].local_latency.size()){
		printf("Warning: the list of operator names does not match up the number of analyzed operators.\n");
		printf("Operators' names will be changed to default names.\n\n");
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

/**
 * Write latency results for n-sources benchmarks
 * 
 * It computes and writes to a file the global average latency of each operator
 * 
 * @param metrics data metrics of a specific source
 * @return nothing.
 */
void write_latency(data_metrics metrics){

	std::string file_name = prepareOutFileAt("log") + "_" + metrics.source_name + "_latency.dat";
	//std::vector<std::string> operator_name_list = set_operators_name();
	FILE *latency_file;
	latency_file = fopen(file_name.c_str(), "w");

	//write the header of the file
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

} //end of namespace spb
