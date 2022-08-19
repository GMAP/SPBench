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
#include <sys/time.h>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <chrono>
#include <cstdlib>
#include <sstream>

#include <stdexcept>

#include <errno.h>
#include <sys/stat.h>
#include <math.h>
#include <string.h>

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
class blocking_queue;

struct data_metrics;
struct item_metrics_data;
struct monitor_data;
struct frequencyPattern_t;

std::string prepareOutFileAt(std::string);
bool file_exists (const std::string&);
volatile unsigned long current_time_usecs();
std::vector<std::string> split_string(const std::string &s, char delim);
void input_freq_pattern_parser(std::string);
data_metrics init_metrics();
void print_throughput(data_metrics);
void print_average_latency(data_metrics);
void write_latency(data_metrics);

float instantLatency(float time_window_in_seconds, long sourceId);
float instantThroughput(float time_window_in_seconds, long sourceId);

void item_frequency_control(unsigned long last_source_item_timestamp, float items_reading_frequency);

void monitor_metrics(unsigned long, unsigned long int);

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

struct item_metrics_data {
	std::vector<double> local_latency;
	long total_latency;
	long item_timestamp;
	long item_sink_timestamp;
	long batch_size;
	item_metrics_data():
		total_latency(0),
		item_timestamp(0),
		item_sink_timestamp(0),
		batch_size(0)
	{}
};

struct monitor_data{
	float timestamp;
	float cpu_usage;
	ssize_t mem_usage;
	float instant_latency;
	float average_latency;
	float instant_throughput;
	float average_throughput;
	float frequency;
	int batch_size;
	monitor_data():
		timestamp(0),
		cpu_usage(0.0),
		mem_usage(0),
		instant_latency(0.0),
		average_latency(0.0),
		instant_throughput(0.0),
		average_throughput(0.0),
		frequency(0.0),
		batch_size(0)
	{}
};

struct data_metrics {
		int *rapl_fd;
		int fd_cache;
		long start_throughput_clock;
		long stop_throughput_clock; // stores the last time measured by an item at sink

		std::vector<item_metrics_data> latency_vector_ns;
		long global_batch_counter; //counted at source
		long global_item_counter; //counted at source

		// for real time metrics
		long batches_at_sink_counter;
		long items_at_sink_counter;
		long global_latency_acc; // latency accumulator
		
		/* monitoring variables */
		std::vector<monitor_data> monitor_vector; //computed at sink
		long monitored_items_counter; //counted at sink
		long last_measured_time;
		
		int sourceId;
		std::string source_name;

	data_metrics():
		rapl_fd(NULL),
		fd_cache(0),
		start_throughput_clock(0),
		stop_throughput_clock(0),
		global_batch_counter(0),
		global_item_counter(0),
		batches_at_sink_counter(0),
		items_at_sink_counter(0),
		global_latency_acc(0),
		monitored_items_counter(0),
		last_measured_time(current_time_usecs()),
		sourceId(0),
		source_name("unnamed")
	{}
};

struct frequencyPattern_t{
		std::string pattern;
		float period;
		float low;
		float high;
		float wavelength;
		float wave_preset;
		float off_set;
		float amplitude;
		float range_freq;
		float step;
		float spikeInterval;
	frequencyPattern_t():
		pattern("none"),
		period(0.0),
		low(0.0),
		high(0.0),
		wavelength(0.0),
		wave_preset(0.0),
		off_set(0.0),
		amplitude(0.0),
		range_freq(0.0),
		step(0.0),
		spikeInterval(0.0)
	{}
};

class SPBench{
private:

	static float items_reading_frequency; //usec precision
	static int batch_size;
	static float batch_interval;
	static bool memory_source;

	static std::vector<std::string> operator_name_list;

	static frequencyPattern_t freq_patt;

	static int number_of_operators;

public:

	static int getNewOpId();

	static void frequency_pattern();

	static void item_frequency_control(unsigned long last_source_item_timestamp);

	static void setFrequencyPattern(std::string freq_pattern, float freq_period, float freq_low, float freq_high, float freq_spike = 10);

	static unsigned long pattern_cycle_start_time;
	
	static std::vector<std::string> userArgs;
	static void setArg(std::string);

	static std::string getArg(int);

	static std::string bench_path; //stores executable name from arg[0] in init_bench()

	SPBench(){}
	virtual ~SPBench(){}

	unsigned long getCurrentTimeUsec(){
		return current_time_usecs();
	}

	static void setFrequency(float _items_reading_frequency){items_reading_frequency = _items_reading_frequency;} //usec precision
	static float getFrequency(){return items_reading_frequency;} //usec precision

	static void setBatchSize(int _batch_size){batch_size = _batch_size;}
	static int getBatchSize(){return batch_size;}

	static void setBatchInterval(float _batch_interval){batch_interval = _batch_interval;}
	static float getBatchInterval(){return batch_interval;}

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

	static long batch_counter; // batches processed at source
	static long items_counter; // items processed at source
	static long items_at_sink_counter;
	static long batches_at_sink_counter;
	static long global_latency_acc;
	static long execution_init_clock;
	static long item_old_time;
	static long monitored_items_counter;
	static long monitoring_time_interval;
	static long last_batch_size;

	struct monitor_data;
	struct item_metrics_data;

	struct monitor_data{
		float timestamp;
		float cpu_usage;
		ssize_t mem_usage;
		float instant_latency;
		float average_latency;
		float instant_throughput;
		float average_throughput;
		float frequency;
		unsigned int batch_size;
	};

	struct data_metrics {
			int *rapl_fd;
			int fd_cache;
			unsigned long start_throughput_clock;
			unsigned long stop_throughput_clock;
			std::vector<item_metrics_data> latency_vector_ns;
		data_metrics():
			rapl_fd(NULL),
			fd_cache(0),
			start_throughput_clock(0),
			stop_throughput_clock(0)
		{}
	};

	struct item_metrics_data {
		std::vector<double> local_latency;
		long total_latency;
		long item_timestamp;
		long item_sink_timestamp;
		long batch_size;
		item_metrics_data():
			total_latency(0),
			item_timestamp(0),
			item_sink_timestamp(0),
			batch_size(0)
		{}
	};
	
	static void monitor_metrics();
	//static bool file_exists (const std::string&);
	static void init();
	static void stop();

	//static volatile unsigned long current_time_usecs();
	static void print_throughput(data_metrics);
	static void print_average_latency();
	static void write_latency(std::string);

	//to store the individual latencies
	static std::vector<item_metrics_data> latency_vector;
	static std::vector<monitor_data> monitor_vector;

	// To store timestamps for computing instant latency and throughput
	static std::vector<unsigned long> timestamps_vec;


	Metrics(){}

	virtual ~Metrics(){}

	static item_metrics_data Latency_t(){
		return item_metrics_data();
	}

	static float getAverageLatency(){
		return batches_at_sink_counter > 0? ((global_latency_acc/batches_at_sink_counter) / 1000.0) : 0.0;
	}
	static float getAverageThroughput(){
		return items_at_sink_counter/((current_time_usecs() - execution_init_clock)/1000000.0);
	}

	static float getInstantThroughput(float time_window_in_seconds);
	static float getInstantLatency(float time_window_in_seconds);

	static float getCPUUsage();
	static float getMemoryUsage();

	static void enable_upl(){upl = true;}
	static bool upl_is_enabled(){return upl;}

	static void enable_throughput(){throughput = true;}
	static bool throughput_is_enabled(){return throughput;}

	static void enable_monitoring(){monitoring = true;}
	static bool monitoring_is_enabled(){return monitoring;}

	static void enable_latency(){latency = true;}
	static bool latency_is_enabled(){return latency;}

	static void set_monitoring_time_interval(long _monitoring_time_interval){monitoring_time_interval = _monitoring_time_interval;}
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

/* This class implement the methods required by nsources benchmarks */
class NsItem {
	protected:
		bool isEmpty;
		bool isLast;
	public:
		int sourceId;

		NsItem():
			isEmpty(true),
			isLast(false)
		{};

		~NsItem(){}

		void setNotEmpty(){
			isEmpty = false;
		}

		bool empty(){
			return isEmpty;
		}

		void setLastItem(){
			isLast = true;
		}

		bool isLastItem(){
			return isLast;
		}
};

/* This class implements the batch infrastructure for the items */
class Batch {
	public:
		std::vector<double> latency_op;
		volatile unsigned long timestamp;
		int batch_size;
		int batch_index;

		Batch(int operators):
			latency_op(operators, 0.0),
			timestamp(0.0),
			batch_size(0),
			batch_index(0)
		{};

		~Batch(){
			latency_op.clear();
		}
};

/* This class implements the main methods used by nsources benchmarks */
class SuperSource{

	protected:
		std::thread source_thread;

		long sourceFrequency;
		int sourceBatchSize;
		float sourceBatchInterval;
		
		bool sourceDepleted;
		bool isRunning;
		std::string sourceName;
		int sourceId;
		data_metrics source_metrics;

		void source_op();
		void printStatus();

		void tryToJoin(){ // If thread Object is Joinable then Join that thread.
			if (source_thread.joinable()){
				source_thread.join();
			}
		}

		void setSourceId(int sourceId){
			this->sourceId = sourceId;
		}

	public:
		static int sourceObjCounter;

		~SuperSource(){}
		
		void setFrequency(long sourceFrequency){
			this->sourceFrequency = (sourceFrequency > 0 ? sourceFrequency : 0);
			return;
		}

		long getFrequency(){
			return this->sourceFrequency;
		}

		void setSourceName(std::string sourceName){
			this->sourceName = sourceName;
			return;
		}

		std::string getSourceName(){
			return sourceName;
		}

		int getSourceId(){
			return sourceId;
		}

		void setSourceDepleted(bool sourceState){
			this->sourceDepleted = sourceState;
		}

		bool getSourceDepleted(){
			return sourceDepleted;
		}

		void setRunningState(bool runningState){
			this->isRunning = runningState;
		}

		bool getRunningState(){
			return isRunning;
		}

		void setBatchSize(int sourceBatchSize){
			this->sourceBatchSize = (sourceBatchSize > 0 ? sourceBatchSize : 1);
			return;
		}

		void setBatchInterval(float sourceBatchInterval){
			this->sourceBatchInterval = (sourceBatchInterval > 0.0 ? sourceBatchInterval : 0.0);
			return;
		}

		float getBatchInterval(){
			return sourceBatchInterval;
		}

		bool depleted(){
			return sourceDepleted;
		}

		float getInstantThroughput(float time_window_in_seconds){
			return sourceDepleted ? 0.0 : instantThroughput(time_window_in_seconds, sourceId);
		}
		float getAverageThroughput(){
			return sourceDepleted ? 0.0 : metrics_vec[sourceId].items_at_sink_counter /((current_time_usecs() - metrics_vec[sourceId].start_throughput_clock)/1000000.0);
		}
		float getInstantLatency(float time_window_in_seconds){
			return sourceDepleted ? 0.0 : instantLatency(time_window_in_seconds, sourceId);
		}
		float getAverageLatency(){
			if(metrics_vec[sourceId].batches_at_sink_counter <= 0){
				return 0.0;
			}
			return sourceDepleted ? 0.0 : (metrics_vec[sourceId].global_latency_acc/metrics_vec[sourceId].batches_at_sink_counter) / 1000.0;
		}
};

} //end of namespace spb


#endif
