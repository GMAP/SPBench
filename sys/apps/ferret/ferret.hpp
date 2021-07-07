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

using namespace std;

//int nthreads = 1;
class Item;

//bool memory_source = false;

inline bool source_op(Item &item);
inline void segmentation_op(Item &item);
inline void extract_op(Item &item);
inline void vectorization_op(Item &item);
inline void rank_op(Item &item);
inline void sink_op(Item &item);

vector<string> set_operators_name();
void init_bench(int argc, char* argv[]);
//inline bool file_exists (const string& name);
inline void usage(string name);
void end_bench();


#ifdef ENABLE_PARSEC_HOOKS
#include <hooks.h>
#endif

#define DEFAULT_DEPTH   25
#define MAXR            100
#define IMAGE_DIM       14

char *db_dir = NULL;
const char *table_name = NULL;
const char *query_dir = NULL;
const char *output_path = NULL;

char m_path[BUFSIZ];
const char *m_single_file;

std::stack<DIR *> m_dir_stack;
std::stack<int>   m_path_stack;

char *extra_params = "-L 8 - T 20";
FILE *fout;
int top_K = 10;

cass_env_t *env;
cass_table_t *table;
cass_table_t *query_table;

int vec_dist_id = 0;
int vecset_dist_id = 0;

int NTHREAD = 1;
int DEPTH = 1;

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
};

struct rank_data
{
	char *name;
	cass_dataset_t *ds;
	cass_result_t result;
};

struct all_data {
	union {
		struct load_data      load;
		struct rank_data      rank;
	} first;
	union {
		struct seg_data       seg;
		struct vec_query_data vec;
	} second;
	struct extract_data extract;
};

struct all_data *file_helper (const char *file)
{
	int r;
	struct all_data *data;

	data = (struct all_data *)malloc(sizeof(struct all_data));
	assert(data != NULL);

	data->first.load.name = strdup(file);

	r = image_read_rgb_hsv(file,
			&data->first.load.width,
			&data->first.load.height,
			&data->first.load.RGB,
			&data->first.load.HSV);
	assert(r == 0);

	return data;
}

void push_dir(const char * dir) {
	int path_len = strlen(m_path);
	DIR *pd = NULL;

	strcat(m_path, dir);
	pd = opendir(m_path);
	if (pd != NULL) {
		strcat(m_path, "/");
		m_dir_stack.push(pd);
		m_path_stack.push(path_len);
	} else {
		m_path[path_len] = 0;
	}
}

void scan(const char * dir) {
	m_path[0] = 0;
	int ret;
	if (strcmp(dir, ".") == 0) {
		m_single_file = NULL;
		push_dir(".");
	} else if (strcmp(dir, "..") == 0) {
		m_single_file = NULL;
	}
	else {
		struct stat st;
		ret = stat(dir, &st);
		if (ret != 0){
			perror("Error:");
			m_single_file = NULL;
		}
		if (S_ISREG(st.st_mode)) {
			m_single_file = dir;
		} else if (S_ISDIR(st.st_mode)) {
			m_single_file = NULL;
			push_dir(dir);
		}
	}
}

class Item{
	public:
		std::vector<all_data*> ret_batch;

		std::vector<cass_query_t> query_batch;

		unsigned int batch_size;
		unsigned int index;

		std::vector<double> latency_op;
		volatile unsigned long timestamp;

		Item():
			batch_size(0),
			latency_op(6, 0.0),
			timestamp(0.0)
		{};

		~Item(){
			latency_op.clear();
			ret_batch.clear();
			query_batch.clear();
		}
};



std::vector<all_data*> ret_in_memory_vector;
int ret_cass;

vector<string> set_operators_name(){
	vector<string> operator_name_list;
	operator_name_list.push_back("Source");
	operator_name_list.push_back("Segmentation");
	operator_name_list.push_back("Extract");
	operator_name_list.push_back("Vectorization");
	operator_name_list.push_back("Rank");
	operator_name_list.push_back("Sink");
	return operator_name_list;
}

inline void usage(string name){
	fprintf(stderr, "Usage: %s\n", name.c_str());
	fprintf(stderr, "\t-i\t: <input_file> (mandatory)\n");
	fprintf(stderr, "\t-b\t: <number_of_itens_per_batch>\n");
	fprintf(stderr, "\t-t\t: <number_of_threads>\n");
	fprintf(stderr, "\t-m\t: read entire input file into memory first\n");
	fprintf(stderr, "\t-l\t: print average latency results\n");
	fprintf(stderr, "\t-f\t: store individual latency values into a file\n");
	fprintf(stderr, "\t-x\t: print average throughput results\n");
	fprintf(stderr, "\t-u\t: print memory consumption results generated by UPL library\n");
	fprintf(stderr, "\t-h\t: print this help message\n");
	exit(-1);
}

void init_bench(int argc, char *argv[]){

	if (argc < 5)
	{
		printf("%s <database> <table> <query dir> <top K> <ignored> <n> <out>\n", argv[0]);
		exit(0);
	}
	
	filepath = argv[0];
	db_dir = argv[1];
	table_name = argv[2];
	query_dir = argv[3];
	top_K = atoi(argv[4]);
	//DEPTH = atoi(argv[5]);
	//NTHREAD = atoi(argv[6]);
	//nthreads = atoi(argv[6]);
	//output_path = argv[5];

	string input;
	int opt;
	while ((opt = getopt(argc,argv,"i:b:t:m:F:klfxuh")) != EOF){
		switch(opt){
			case 'i':
				input = optarg;
				break;
			case 't':
				nthreads = atoi(optarg);
				break;
			case 'b':
				batch_size = atoi(optarg);
				break;
			case 'k':
				memory_source = true;
				break;
			case 'm':
				monitoring_time_interval = atoi(optarg);
				enable_monitoring = true;
				break;
			case 'F':
				items_reading_frequency = atoi(optarg);
				memory_source = true;
				break;
			case 'l':
				print_latency = true;
				break;
			case 'f':
				latency_to_file = true;
				break;
			case 'x':
				enable_throughput = true;
				break;
			case 'u':
				enable_upl = true;
				break;
			case 'h':
				usage(argv[0]);
				break;
			case '?': 
				usage(argv[0]);
				break;
			default: 
				cout << endl; 
				exit(1);
		}
	}

	int i;

	fout = fopen((out_file_path("outputs") + ".out").c_str(), "w");
	assert(fout != NULL);

	cass_init();
	ret_cass = cass_env_open(&env, db_dir, 0);
	if (ret_cass != 0) {
		printf("ERROR: %s\n", cass_strerror(ret_cass));
		exit(0);
	}

	vec_dist_id = cass_reg_lookup(&env->vec_dist, "L2_float");
	assert(vec_dist_id >= 0);

	vecset_dist_id = cass_reg_lookup(&env->vecset_dist, "emd");
	assert(vecset_dist_id >= 0);

	i = cass_reg_lookup(&env->table, table_name);
	query_table = (cass_table_t*) cass_reg_get(&env->table, i);
	table = query_table;

	i = table->parent_id;
	if (i >= 0) {
		query_table = (cass_table_t*) cass_reg_get(&env->table, i);
	}

	if (query_table != table) cass_table_load(query_table);

	cass_map_load(query_table->map);

	cass_table_load(table);

	image_init(argv[0]);

	scan(query_dir);

	if(memory_source){
		while(1){

			struct all_data* ret;
			bool input_found = false;

			if(m_single_file) {
				ret = file_helper(m_single_file);
				m_single_file = NULL;
				input_found = true;
			}

			if(m_dir_stack.empty()) {
				stream_end = true;
				break;
			}

			while(input_found!=true) {
				DIR *pd = m_dir_stack.top();
				struct dirent *ent = NULL;
				int res = 0;
				struct stat st;
				int path_len = strlen(m_path);

				ent = readdir(pd);
				if (ent == NULL) {
					closedir(pd);
					m_path[m_path_stack.top()] = 0;
					m_path_stack.pop();
					m_dir_stack.pop();
					if(m_dir_stack.empty()) {
						stream_end = true;
						break;
					}
				}

				if((ent->d_name[0] == '.') && ((ent->d_name[1] == 0) || ((ent->d_name[1] == '.') && (ent->d_name[2] == 0)) ) )
					continue;

				strcat(m_path, ent->d_name);
				res = stat(m_path, &st);
				if (res != 0) {
					perror("Error:");
					stream_end = true;
					break;
				}
				if (S_ISREG(st.st_mode)) {
					ret = file_helper(m_path);
					m_path[path_len]=0;
					input_found = true;
				} else if (S_ISDIR(st.st_mode)) {
					m_path[path_len]=0;
					push_dir(ent->d_name);
				} else
					m_path[path_len]=0;
			}
		
			if(stream_end == true) break;

			ret_in_memory_vector.push_back(ret);
		}
		stream_end = false;
	}
}

void end_bench(){

	if(memory_source){
		unsigned int num_item = 0;
		while(num_item < ret_in_memory_vector.size()){
			struct all_data* ret;
			ret = ret_in_memory_vector[num_item];
			
			fprintf(fout, "%s", ret->first.rank.name);
			do {
				cass_size_t __array_foreach_index;
				for (__array_foreach_index = 0; __array_foreach_index < ret->first.rank.result.u.list.len;
						__array_foreach_index++) {
					cass_list_entry_t *p = ret->first.rank.result.u.list.data + __array_foreach_index;
					char *obj = NULL;
					if (p->dist == DBL_MAX) continue;
					cass_map_id_to_dataobj(query_table->map, p->id, &obj);
					assert(obj != NULL);
					fprintf(fout, "\t%s:%g", obj, p->dist);
				}
			} while(0);

			fprintf(fout, "\n");

			cass_result_free(&ret->first.rank.result);
			free(ret->first.rank.name);
			free(ret);
			num_item++;
		}
		
		if(!ret_in_memory_vector.empty())
			ret_in_memory_vector.erase(ret_in_memory_vector.begin(), ret_in_memory_vector.end());
	}


	ret_cass = cass_env_close(env, 0);
	if (ret_cass != 0) {
		printf("ERROR: %s\n", cass_strerror(ret_cass));
		exit(0);
	}

	cass_cleanup();

	image_cleanup();

	fclose(fout);

}

inline bool source_op(Item &item){

	//if last batch included the last item, ends computation
	if(stream_end == true){
		return false;
	}

	//metrics computation
	volatile unsigned long latency_op;
	item.timestamp = current_time_usecs();
	if(print_latency || latency_to_file){
		latency_op = current_time_usecs();
	}

	while(item.batch_size < batch_size){ //batch loop
		
		if(memory_source){
			if(item_count < ret_in_memory_vector.size()){

				struct all_data *ret;
				item.ret_batch.resize(item.batch_size+1);
				item.ret_batch[item.batch_size] = ret_in_memory_vector[item_count];
			} else {
				stream_end = true;
				break;
			}
		} else {

			struct all_data* ret;
			bool input_found = false;

			if(m_single_file) {
				ret = file_helper(m_single_file);
				m_single_file = NULL;
				input_found = true;
			}

			if(m_dir_stack.empty()) {
				stream_end = true;
				break;
			}

			while(input_found!=true) {
				DIR *pd = m_dir_stack.top();
				struct dirent *ent = NULL;
				int res = 0;
				struct stat st;
				int path_len = strlen(m_path);

				ent = readdir(pd);
				if (ent == NULL) {
					closedir(pd);
					m_path[m_path_stack.top()] = 0;
					m_path_stack.pop();
					m_dir_stack.pop();
					if(m_dir_stack.empty()) {
						stream_end = true;
						break;
					}
				}

				if((ent->d_name[0] == '.') && ((ent->d_name[1] == 0) || ((ent->d_name[1] == '.') && (ent->d_name[2] == 0)) ) )
					continue;

				strcat(m_path, ent->d_name);
				res = stat(m_path, &st);
				if (res != 0) {
					perror("Error:");
					stream_end = true;
					break;
				}
				if (S_ISREG(st.st_mode)) {
					ret = file_helper(m_path);
					m_path[path_len]=0;
					input_found = true;
				} else if (S_ISDIR(st.st_mode)) {
					m_path[path_len]=0;
					push_dir(ent->d_name);
				} else
					m_path[path_len]=0;
			}

			if(stream_end == true){
				break;
			}
			item.ret_batch.push_back(ret);
		}
		item.batch_size++;
		item_count++;
	}

	//if this batch has size 0, ends computation
	if(item.batch_size == 0){
		return false;
	}

	// frequency control mechanism
	item_frequency_control(item.timestamp);
	
	//metrics computation
	if(print_latency || latency_to_file){
		item.latency_op[0] = (current_time_usecs() - latency_op) / 1000000.0;
	}
	input_items++;
	return true;
}

inline void segmentation_op(Item &item){
	//metrics computation
	volatile unsigned long latency_op;
	if(print_latency || latency_to_file){
		latency_op = current_time_usecs();
	}

	unsigned int num_item = 0;
	while(num_item < item.batch_size){ //batch loop
		struct all_data* ret;
		ret = item.ret_batch[num_item];

		ret->second.seg.name = ret->first.load.name;
		ret->second.seg.width = ret->first.load.width;
		ret->second.seg.height = ret->first.load.height;
		ret->second.seg.HSV = ret->first.load.HSV;
		image_segment((void**)&ret->second.seg.mask,
				&ret->second.seg.nrgn,
				ret->first.load.RGB,
				ret->first.load.width,
				ret->first.load.height);
		free(ret->first.load.RGB);

		item.ret_batch[num_item] = ret;
		num_item++;
	}
	//metrics computation
	if(print_latency || latency_to_file){
		item.latency_op[1] = (current_time_usecs() - latency_op) / 1000000.0;
	}
}

inline void extract_op(Item &item){
	//metrics computation
	volatile unsigned long latency_op;
	if(print_latency || latency_to_file){
		latency_op = current_time_usecs();
	}
	
	unsigned int num_item = 0;
	while(num_item < item.batch_size){ //batch loop
		struct all_data* ret;
		ret = item.ret_batch[num_item];

		ret->extract.name = ret->second.seg.name;
		image_extract_helper(ret->second.seg.HSV,
				ret->second.seg.mask,
				ret->second.seg.width,
				ret->second.seg.height,
				ret->second.seg.nrgn,
				&ret->extract.ds);
		free(ret->second.seg.mask);
		free(ret->second.seg.HSV);
		item.ret_batch[num_item] = ret;
		num_item++;
	}
	//metrics computation
	if(print_latency || latency_to_file){
		item.latency_op[2] = (current_time_usecs() - latency_op) / 1000000.0;
	}
}

inline void vectorization_op(Item &item){
	//metrics computation
	volatile unsigned long latency_op;
	if(print_latency || latency_to_file){
		latency_op = current_time_usecs();
	}
	
	unsigned int num_item = 0;
	while(num_item < item.batch_size){ //batch loop
		struct all_data* ret;
		ret = item.ret_batch[num_item];

		cass_query_t query;
		query = item.query_batch[num_item];

		ret->second.vec.name = ret->extract.name;

		memset(&query, 0, sizeof query);
		query.flags = CASS_RESULT_LISTS | CASS_RESULT_USERMEM;

		ret->second.vec.ds = query.dataset = &ret->extract.ds;
		query.vecset_id = 0;

		query.vec_dist_id = vec_dist_id;

		query.vecset_dist_id = vecset_dist_id;

		query.topk = 2*top_K;

		query.extra_params = extra_params;

		cass_result_alloc_list(&ret->second.vec.result,
				ret->second.vec.ds->vecset[0].num_regions,
				query.topk);

		cass_table_query(table, &query, &ret->second.vec.result);
		
		item.ret_batch[num_item] = ret;
		item.query_batch.push_back(query);
		num_item++;
	}
	//metrics computation
	if(print_latency || latency_to_file){
		item.latency_op[3] = (current_time_usecs() - latency_op) / 1000000.0;
	}
}

inline void rank_op(Item &item){
	//metrics computation
	volatile unsigned long latency_op;
	if(print_latency || latency_to_file){
		latency_op = current_time_usecs();
	}
	unsigned int num_item = 0;
	while(num_item < item.batch_size){ //batch loop
		struct all_data* ret;
		ret = item.ret_batch[num_item];

		cass_query_t query;
		query = item.query_batch[num_item];

		cass_result_t *candidate;

		ret->first.rank.name = ret->second.vec.name;

		query.flags = CASS_RESULT_LIST | CASS_RESULT_USERMEM | CASS_RESULT_SORT;
		query.dataset = ret->second.vec.ds;
		query.vecset_id = 0;

		query.vec_dist_id = vec_dist_id;

		query.vecset_dist_id = vecset_dist_id;

		query.topk = top_K;

		query.extra_params = NULL;

		candidate = cass_result_merge_lists(&ret->second.vec.result,
				(cass_dataset_t *)query_table->__private,
				0);
		query.candidate = candidate;

		cass_result_alloc_list(&ret->first.rank.result,
				0, top_K);
		cass_table_query(query_table, &query,
				&ret->first.rank.result);

		cass_result_free(&ret->second.vec.result);
		cass_result_free(candidate);
		free(candidate);
		cass_dataset_release(ret->second.vec.ds);
		item.ret_batch[num_item] = ret;
		num_item++;
	}
	//metrics computation
	if(print_latency || latency_to_file){
		item.latency_op[4] = (current_time_usecs() - latency_op) / 1000000.0;
	}
}

inline void sink_op(Item &item){
	//metrics computation
	volatile unsigned long latency_op;
	if(print_latency || latency_to_file){
		latency_op = current_time_usecs();
	}

	unsigned int num_item = 0;
	while(num_item < item.batch_size){ //batch loop
		if(memory_source){
			break;
		} else {

			struct all_data* ret;
			ret = item.ret_batch[num_item];
			
			fprintf(fout, "%s", ret->first.rank.name);
			do {
				cass_size_t __array_foreach_index;
				for (__array_foreach_index = 0; __array_foreach_index < ret->first.rank.result.u.list.len;
						__array_foreach_index++) {
					cass_list_entry_t *p = ret->first.rank.result.u.list.data + __array_foreach_index;
					char *obj = NULL;
					if (p->dist == DBL_MAX) continue;
					cass_map_id_to_dataobj(query_table->map, p->id, &obj);
					assert(obj != NULL);
					fprintf(fout, "\t%s:%g", obj, p->dist);
				}
			} while(0);

			fprintf(fout, "\n");

			cass_result_free(&ret->first.rank.result);
			free(ret->first.rank.name);
			free(ret);
		}
		num_item++;
	}

	//metrics computation
	if(enable_monitoring){
		monitor_metrics(item.timestamp);
	}
	if(print_latency || latency_to_file){
		item.latency_op[5] = (current_time_usecs() - latency_op) / 1000000.0;
		latency_t latency;
		latency.local_latency = item.latency_op;
		latency.local_total = ((current_time_usecs() - item.timestamp) / 1000000.0);
		latency_vector.push_back(latency);
		item.latency_op.clear();
	}
}
