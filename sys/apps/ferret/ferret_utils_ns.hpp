/** 
 * ************************************************************************  
 *  File  : ferret.hpp
 *
 *  Title : SPBench version of the Ferret application
 *
 *  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 *
 *  Date  : July 06, 2021
 *
 * ************************************************************************
**/

/** 
 * Copyright (C) 2007 Princeton University
 *       
 * This file is part of Ferret Toolkit.
 * 
 * Ferret Toolkit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
**/

#ifndef FERRET_U
#define FERRET_U

#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include <stack>
#include "include/cass.h"
#include "include/cass_timer.h"
#include "image/image.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <sys/time.h>

#include <spbench.hpp>

#ifdef ENABLE_PARSEC_HOOKS
#include <hooks.h>
#endif

#define DEFAULT_DEPTH   25
#define MAXR            100
#define IMAGE_DIM       14

#define NUMBER_OF_OPERATORS 6

namespace spb{

class Item;
class Source;
class Sink;

struct input_data;
struct output_data;
struct path_data;
struct IO_data_struct;

struct load_data;
struct seg_data;
struct extract_data;
struct vec_query_data;
struct rank_data;
struct item_data;

extern std::vector<IO_data_struct> IO_data_vec;
extern std::vector<input_data> input_data_vec;

void init_bench(int argc, char* argv[]);
void end_bench();

extern char *extra_params;
extern unsigned int top_K;

extern cass_table_t *table;
extern cass_table_t *query_table;

extern int vec_dist_id;
extern int vecset_dist_id;

struct path_data{
	char m_path[BUFSIZ];
	const char *m_single_file;

	std::stack<DIR *> m_dir_stack;
	std::stack<int>   m_path_stack;
};

struct load_data
{
	int width, height;
	char *name;
	unsigned char *HSV, *RGB;
};

struct seg_data
{
	int width, height, nrgn;
	char *name;
	unsigned char *mask;
	unsigned char *HSV;
};

struct extract_data
{
	cass_dataset_t ds;
	char *name;
};

struct vec_query_data
{
	char *name;
	cass_dataset_t *ds;
	cass_result_t result;
	cass_query_t query;
};

struct rank_data
{
	char *name;
	cass_dataset_t *ds;
	cass_result_t result;
	cass_query_t query;
};

struct item_data {
	union {
		struct load_data      load;
		struct rank_data      rank;
	} first;
	union {
		struct seg_data       seg;
		struct vec_query_data vec;
	} second;
	struct extract_data extract;
	unsigned int index;
	unsigned int sourceId;

	item_data():
		index(0)
	{};

	~item_data(){};
};

struct output_data {
	FILE * fout;
	int ret_cass;
	cass_env_t *env;
	output_data():
		fout(NULL),
		ret_cass(0),
		env(NULL)
	{}
};

struct input_data{
	char *db_dir = NULL;
	const char *table_name = NULL;
	const char *query_dir = NULL;
	int top_K;
	std::string inputId;

	cass_table_t *table;
	cass_table_t *query_table;
	int vec_dist_id;
	int vecset_dist_id;
	
	input_data():
		top_K(25)
	{}
};

struct IO_data_struct {
	input_data inputData;
	output_data outFileData;
	std::vector<item_data*> mem_data_vec;	
};

class Item : public Batch, public NsItem{
public:
	std::vector<item_data*> item_batch;

	Item():Batch(NUMBER_OF_OPERATORS){};

	~Item(){}
};


class Source : public SuperSource{
	private:
		unsigned int sourceQueueSize;
		
		void source_op();
		void printStatus();

		void tryToJoin(){ // If thread Object is Joinable then Join that thread.
			if (source_thread.joinable()){
				source_thread.join();
			}
		}

		void getInput(){
			if(input_data_vec.size() < sourceObjCounter){
				std::cout << " Error!! You must provide an input for each source." << std::endl;
				std::cout << " - Created sources: " << sourceObjCounter << std::endl;
				std::cout << " - Given inputs: " << input_data_vec.size() << std::endl;
				std::cout << std::endl;
				exit(0);
			}
			IO_data_vec.push_back({input_data_vec[sourceId]});
			return;
		}

	public:
		std::vector<Metrics::item_metrics_data> sourceLatencyVector;
		concurrent::queue::blocking_queue<Item> sourceQueue;

		Source() : SuperSource(){
			setBatchSize(SPBench::getBatchSize());
			setBatchInterval(SPBench::getBatchInterval());
			setQueueMaxSize(0);
			setFrequency(SPBench::getFrequency());
			setSourceId(sourceObjCounter);
			setSourceDepleted(false);
			setRunningState(false);
			sourceObjCounter++;
		}

		Source(int batchSize, float batchInterval, int queueSize, long frequency) : SuperSource(){
			sourceDepleted = false;
			sourceId = sourceObjCounter;
			sourceObjCounter++;
			getInput();
			setFrequency(frequency);
			setQueueMaxSize(queueSize);
			setBatchSize(batchSize);
			setSourceName(IO_data_vec[sourceId].inputData.inputId);
			source_metrics = init_metrics();
			source_metrics.sourceId = sourceId;
			source_metrics.source_name = getSourceName();
			metrics_vec.push_back(source_metrics);
			source_thread = std::thread(&Source::source_op, this);
			isRunning = true;
			printStatus();
		}

		~Source(){}
		
		void init(){
			if(isRunning){
				std::cout << "Failed to use init(), this source is already running!" << std::endl;
				return;
			}

			getInput();
			setSourceName(IO_data_vec[sourceId].inputData.inputId);

			source_metrics = init_metrics();
			source_metrics.sourceId = sourceId;
			source_metrics.source_name = getSourceName();
			metrics_vec.push_back(source_metrics);
			source_thread = std::thread(&Source::source_op, this);
			isRunning = true;

			printStatus();
		}

		Item getItem(){
			
			Item item;
			
			if(depleted() || !isRunning) return item;

			item = sourceQueue.dequeue();
			item.sourceId = sourceId;

			if (sourceQueue.front().isLastItem()){
				std::cout << "\n -> Source depleted: " << getSourceName() << std::endl;
				sourceDepleted = true;
				tryToJoin();
			}
			return item;
		}

		void setQueueMaxSize(unsigned int newQueueSize){
			
			if(newQueueSize < 0){
				std::cout << " Queue size must be 0 (unlimited) or higher.\n";
				return;
			}

			if(newQueueSize < sourceQueue.size()){
				std::cout << " You cannot set a max. queue size lower than the number of current items in the queue.\n";
				std::cout << " Number of items in the queue: " << sourceQueue.size() << std::endl;
				return;
			}

			sourceQueueSize = newQueueSize;
			sourceQueue.setCapacity(sourceQueueSize);
		}
};

class Sink{
public:
	static void op(Item &item);
	Sink(){}
	virtual ~Sink(){}
};

} //end of namespace spb

#endif