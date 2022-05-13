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

#ifndef SPBENCH_H
#define SPBENCH_H 

#include <vector>
#include <string>
#include <iostream>
#include <sys/time.h>
#include <fstream>
//#include <upl.h>
#include <unistd.h>
#include <filesystem>

/* queue */
#pragma once
#include <iostream>
#include <assert.h>    
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#define MAX_CAPACITY 50

namespace spb{

extern unsigned int nthreads;

class SPBench;
class Metrics;

struct data_metrics;
struct latency_t;
struct monitor_data;

std::string prepareOutFileAt(std::string);
bool file_exists (const std::string&);
volatile unsigned long current_time_usecs();
data_metrics init_metrics();
void print_throughput(data_metrics);
void print_average_latency(data_metrics);
void write_latency(data_metrics);

void item_frequency_control(volatile unsigned long, volatile unsigned long);
void monitor_metrics(volatile unsigned long, unsigned long int);

void compute_metrics();

extern std::vector<data_metrics> metrics_vec;

//parser to retrieve the executable name from the full path
template<class T>
T base_name(T const & path, T const & delims = "//")
{
	return path.substr(path.find_last_of(delims) + 1);
}

template<class T>
T remove_extension(T const & filename)
{
	typename T::size_type const p(filename.find_last_of('.'));
	return p > 0 && p != T::npos ? filename.substr(0, p) : filename;
}

struct latency_t {
	std::vector<double> local_latency;
	double local_total;
	latency_t():
		local_total(0.0)
	{}
};

struct monitor_data{
	float timestamp;
	float cpu_usage;
	ssize_t mem_usage;
	float latency_item;
	float throughput;
	unsigned long frequency;
};

struct data_metrics {
		int *rapl_fd;
		int fd_cache;
		volatile unsigned long start_throughput_clock;
		volatile unsigned long stop_throughput_clock; // stores the last time measured by an item at sink

		std::vector<latency_t> latency_vector_ns;
		unsigned long global_batch_counter; //counted at source
		unsigned long global_item_counter; //counted at source

		// for real time metrics
		unsigned long batches_at_sink_counter;
		volatile unsigned long global_latency_acc; // latency accumulator
		volatile unsigned long global_current_latency; // single item latency
		
		/* monitoring variables */
		std::vector<monitor_data> monitor_vector; //computed at sink
		unsigned long monitored_items_counter; //counted at sink
		volatile unsigned long last_measured_time;
		
		unsigned int sourceId;
		std::string source_name;
	data_metrics():
		rapl_fd(NULL),
		fd_cache(0),
		start_throughput_clock(0),
		stop_throughput_clock(0),
		global_batch_counter(0),
		global_item_counter(0),
		batches_at_sink_counter(1),
		global_latency_acc(0),
		global_current_latency(0),
		monitored_items_counter(0),
		last_measured_time(current_time_usecs()),
		sourceId(0),
		source_name("unnamed")
	{}
};

class SPBench{
private:

	static unsigned long items_reading_frequency; //usec precision
	static unsigned int batch_size;
	static bool memory_source;

	static std::vector<std::string> operator_name_list;

public:
	static void item_frequency_control(volatile unsigned long);
	
	static std::vector<std::string> userArgs;
	static void setArg(std::string);
	//static void init_metrics();
	//static void stop_metrics();
	static std::string getArg(unsigned int);

	static std::string bench_path; //stores executable name from arg[0] in init_bench()

	SPBench(){}
	virtual ~SPBench(){}

	volatile unsigned long getCurrentTimeUsec(){
		return current_time_usecs();
	}

	static void set_items_reading_frequency(unsigned long _items_reading_frequency){items_reading_frequency = _items_reading_frequency;} //usec precision
	static unsigned long get_items_reading_frequency(){return items_reading_frequency;} //usec precision

	static void set_batch_size(unsigned int _batch_size){batch_size = _batch_size;}
	static unsigned int get_batch_size(){return batch_size;}

	static void enable_memory_source(){memory_source = true;}
	static bool memory_source_is_enabled(){return memory_source;}

	static void addOperatorName(std::string opName){ operator_name_list.push_back(opName); }
	static std::vector<std::string> get_operator_name_list(){return operator_name_list;}

	static std::string getExecPath(){ return bench_path; }

};

class Metrics {
public:

	struct data_metrics;

	static data_metrics metrics;

	static bool upl;
	static bool print_latency;
	static bool latency_to_file;
	static bool throughput;
	static bool monitoring;
	static bool latency;

	static unsigned long batch_counter; // batches processed
	static unsigned long items_counter;
	static unsigned long items_at_sink_counter;
	static volatile unsigned long start_time_usec;
	static volatile unsigned long end_time_usec;
	static volatile unsigned long global_latency_acc;
	static volatile unsigned long global_current_latency;
	static volatile unsigned long execution_init_clock;
	static volatile unsigned long item_old_time;
	static unsigned int monitored_items_counter;
	static unsigned long monitoring_time_interval;


	struct monitor_data;
	struct latency_t;
	struct data_metrics;

	struct monitor_data{
		float timestamp;
		float cpu_usage;
		ssize_t mem_usage;
		float latency_item;
		float throughput;
		unsigned long frequency;
	};

	struct data_metrics {
			int *rapl_fd;
			int fd_cache;
			volatile unsigned long start_throughput_clock;
			volatile unsigned long stop_throughput_clock;
			std::vector<latency_t> latency_vector_ns;
		data_metrics():
			rapl_fd(NULL),
			fd_cache(0),
			start_throughput_clock(0),
			stop_throughput_clock(0)
		{}
	};

	struct latency_t {
		std::vector<double> local_latency;
		volatile unsigned long local_total;
		latency_t():
			local_total(0)
		{}
	};
	
	static void monitor_metrics(volatile unsigned long);
	//static bool file_exists (const std::string&);
	static void init();
	static void stop();

	//static volatile unsigned long current_time_usecs();
	static void print_throughput(data_metrics);
	static void print_average_latency();
	static void write_latency(std::string);

	//to store the individual latencies
	static std::vector<latency_t> latency_vector;
	static std::vector<monitor_data> monitor_vector;

	Metrics(){}

	virtual ~Metrics(){}

	static latency_t getLatency_t(){
		return latency_t();
	}

	static float getAverageLatencyMs(){
		return ((global_latency_acc/items_at_sink_counter) / 1000.0);
	}

	static float getCurrentLatencyMs(){
		return (global_current_latency / 1000.0);
	}

	static float getItemsPerSec(){
		return items_counter/((current_time_usecs() - execution_init_clock)/1000000.0);
	}

	static void enable_upl(){upl = true;}
	static bool upl_is_enabled(){return upl;}

	static void enable_throughput(){throughput = true;}
	static bool throughput_is_enabled(){return throughput;}

	static void enable_monitoring(){monitoring = true;}
	static bool monitoring_is_enabled(){return monitoring;}

	static void enable_latency(){latency = true;}
	static bool latency_is_enabled(){return latency;}

	void item_frequency_control(volatile unsigned long item_timestamp);

	static void set_monitoring_time_interval(unsigned long _monitoring_time_interval){monitoring_time_interval = _monitoring_time_interval;}
	static unsigned long get_monitoring_time_interval(){return monitoring_time_interval;}

	static void enable_print_latency(){
		print_latency = true;
		enable_latency();
	}
	static bool print_latency_is_enabled(){return print_latency;}

	static void enable_latency_to_file(){
		latency_to_file = true;
		enable_latency();
	}
	static bool latency_to_file_is_enabled(){return latency_to_file;}

}; // end of SPBench class


//float getPotentialThroughput(){
//	return (1000000.0/global_current_latency);
//}

} //end of namespace spb

//Precisa ver a questão de licença dessa fila
namespace concurrent {
namespace queue {

template<typename T>
class blocking_queue {
    private:
        //DISABLE_COPY_AND_ASSIGN(blocking_queue);
        blocking_queue(const blocking_queue& rhs);
        blocking_queue& operator= (const blocking_queue& rhs);

    private:
        mutable std::mutex mtx;
        std::condition_variable full_;
        std::condition_variable empty_;
        std::queue<T> queue_;
        size_t capacity_; 

    public:
        blocking_queue() :
            mtx(), 
            full_(), 
            empty_(),
            capacity_(0) 
        {}
        
        void enqueue(const T& data) {
            
			if(capacity_ > 0){
				std::unique_lock<std::mutex> lock(mtx);
				while(queue_.size() == capacity_){
					full_.wait(lock);
				}
				assert(queue_.size() < capacity_);
			}

			queue_.push(data);
            empty_.notify_all(); 
        }
        // pops from the front of the queue
        T dequeue() {
            std::unique_lock<std::mutex> lock(mtx);
            while(queue_.empty()) {
                empty_.wait(lock);
            }

            assert(!queue_.empty());
            T front(queue_.front()); // store the value
            queue_.pop(); // pop it from start
            full_.notify_all();
            return front;
        }

        T front() {
            std::unique_lock<std::mutex> lock(mtx);
            while(queue_.empty()) {
                empty_.wait(lock );
            }

            assert(!queue_.empty());
            T front(queue_.front());
            return front;
        }

        T back() {
            std::unique_lock<std::mutex> lock(mtx);
            while(queue_.empty()) {
                empty_.wait(lock );
            }

            assert(!queue_.empty());
            T back(queue_.back());
            return back;
        }

        size_t size() {
            std::lock_guard<std::mutex> lock(mtx);
            return queue_.size();
        }

        bool empty() {
            std::unique_lock<std::mutex> lock(mtx);
            return queue_.empty();
        }

        void setCapacity(const size_t capacity) {
            capacity_ = (capacity >= 0 ? capacity : 0);
        }
};

}
}

#endif

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

