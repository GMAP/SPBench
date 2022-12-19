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

#include <ferret_utils.hpp>

namespace spb{
void input_parser(char * argv);

void set_operators_name();
void end_bench();


char *db_dir = NULL;
const char *table_name = NULL;
const char *query_dir = NULL;
const char *output_path = NULL;
const char *input_id = NULL;

char m_path[BUFSIZ];
const char *m_single_file;

std::stack<DIR *> m_dir_stack;
std::stack<int>   m_path_stack;

char *extra_params = "-L 8 - T 20";
FILE *fout;
unsigned int top_K = 10;

cass_env_t *env;
cass_table_t *table;
cass_table_t *query_table;

int vec_dist_id = 0;
int vecset_dist_id = 0;

std::vector<item_data*> ret_in_memory_vector;
int ret_cass;

struct item_data *file_helper (const char *);
void push_dir(const char *);
void scan(const char *);

bool stream_end = false;

struct item_data *file_helper (const char *file)
{
	int r;
	struct item_data *data;

	data = (struct item_data *)malloc(sizeof(struct item_data));
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
		strcat(m_path, "//");
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

void set_operators_name(){
	SPBench::addOperatorName("Source       ");
	SPBench::addOperatorName("Segmentation ");
	SPBench::addOperatorName("Extract      ");
	SPBench::addOperatorName("Vectorization");
	SPBench::addOperatorName("Rank         ");
	SPBench::addOperatorName("Sink         ");
}

void usage(std::string name){
	fprintf(stderr, "Usage: %s\n", name.c_str());
	fprintf(stderr, "  -i, --input            \"<db_dir> <table_name> <query_dir> <top_K> <id (optional)>\" (mandatory)\n");
	printGeneralUsage();
	exit(-1);
}


void input_parser(char * argv){ 
	db_dir = strtok (argv," ");
	table_name = strtok (NULL," ");
	query_dir = strtok (NULL," ");
	top_K = atoi(strtok (NULL," "));
	input_id = strtok (NULL," ");
}

void init_bench(int argc, char *argv[]){

	/*if (argc < 5)
	{
		printf("%s <database> <table> <query dir> <top K> <ignored> <n> <out>\n", argv[0]);
		exit(0);
	}
	*/
	SPBench::bench_path = argv[0];
	int opt = 0;
	int opt_index = 0;

	if(argc < 2) usage(argv[0]);
	
	try {
		while ((opt = getopt_long(argc, argv, "i:t:b:B:m:M:f:F:IlLTru:h", long_opts, &opt_index)) != -1) {
			switch(opt){
				case 'i':
					if(split_string(optarg, ' ').size() < 4) 
						throw std::invalid_argument("\n ARGUMENT ERROR --> Invalid input. Required: -i \"<db_dir> <table_name> <query_dir> <top_K> <id (optional)>\"\n");
					input_parser(optarg);
					break;
				case 't':
					nthreads = atoi(optarg);
					break;
				case 'b':
					if (atoi(optarg) <= 0)
						throw std::invalid_argument("\n ARGUMENT ERROR (-b <batch_size>) --> Batch size must be an integer positive value higher than zero!\n");
					SPBench::setBatchSize(atoi(optarg));
					break;
				case 'B':
					if (atof(optarg) <= 0.0)
						throw std::invalid_argument("\n ARGUMENT ERROR (-B <batch_interval>) --> Batch interval must be a value higher than zero!\n");
					SPBench::setBatchInterval(atof(optarg));
				case 'm':
					if (Metrics::monitoring_thread_is_enabled())
						throw std::invalid_argument("\n ARGUMENT ERROR --> You can not use both -m and -M parameters at once.\n");
					Metrics::set_monitoring_time_interval(atoi(optarg));
					Metrics::enable_monitoring();
					break;
				case 'M':
					if (Metrics::monitoring_is_enabled())
						throw std::invalid_argument("\n ARGUMENT ERROR --> You can not use both -m and -M parameters at once.\n");
					Metrics::set_monitoring_time_interval(atoi(optarg));
					Metrics::enable_monitoring_thread();
					break;
				case 'f':
					if (atof(optarg) <= 0.0)
						throw std::invalid_argument("\n ARGUMENT ERROR (-f <frequency>) --> Frequency value must be a positive value higher than zero!\n");
					SPBench::setFrequency(atof(optarg));
					break;
				case 'F':
					input_freq_pattern_parser(optarg);
					break;
				case 'I':
					SPBench::enable_memory_source();
					break;
				case 'l':
					Metrics::enable_print_latency();
					break;
				case 'L':
					Metrics::enable_latency_to_file();
					break;
				case 'T':
					Metrics::enable_throughput();
					break;
				case 'r':
					Metrics::enable_upl();
					break;
				case 'u':
					SPBench::setArg(optarg);
					break;
				case 'h':
					usage(argv[0]);
					break;
				case '?': 
					usage(argv[0]);
					break;
				default: 
					//std::cout << std::endl; 
					usage(argv[0]);
					exit(1);
			}
		}
	} catch(const std::invalid_argument& e) {
		std::cerr << "exception: " << e.what() << std::endl;
		printf("You can use -h to see more options.\n");
		exit(1);
	} catch (const char* msg) {
		std::cerr << "exception: " << msg << std::endl;
		printf("You can use -h to see more options.\n");
		exit(1);
	} catch(...) {
		exit(1);
	}

	int i;

	fout = fopen((prepareOutFileAt("outputs") + "_" + input_id + ".out").c_str(), "w");
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

	if(SPBench::memory_source_is_enabled()){
		while(1){

			struct item_data* ret;
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

	set_operators_name();
	Metrics::enable_latency();
	
	if(Metrics::monitoring_thread_is_enabled()){
		Metrics::start_monitoring();
	}
}

void end_bench(){

	if(SPBench::memory_source_is_enabled()){
		unsigned int num_item = 0;
		while(num_item < ret_in_memory_vector.size()){
			struct item_data* ret;
			ret = ret_in_memory_vector[num_item];
			
			// Removing the full path of each query image from the output file
			// Removing the varying path allows to run correctness testing using md5 hash
			// Original form: fprintf(fout, "%s", ret->first.rank.name);
			std::string aux = ret->first.rank.name;
			std::size_t found = aux.find_last_of("//");
			fprintf(fout, "queries/%s", aux.substr(found+1).c_str());
			
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

long Source::source_item_timestamp = current_time_usecs();

bool Source::op(Item &item){

	//if last batch included the last item, ends computation
	if(stream_end == true){
		return false;
	}

	// frequency control mechanism
	SPBench::item_frequency_control(source_item_timestamp);

	item.timestamp = source_item_timestamp = current_time_usecs();
	unsigned long batch_elapsed_time = source_item_timestamp;
	
	unsigned long latency_op;
	if(Metrics::latency_is_enabled()){
		latency_op = source_item_timestamp;
	}

	while(1) { //main source loop
		// batching management routines
		if(SPBench::getBatchInterval()){
			// Check if the interval of this batch is higher than the batch interval defined by the user
			if(((current_time_usecs() - batch_elapsed_time) / 1000.0) >= SPBench::getBatchInterval()) break;
		} else {
			// If no batch interval is set, than try to close it by size
			if(item.batch_size >= SPBench::getBatchSize()) break;
		}
		// This couples with batching interval to close the batch by size if a size higher than one is defined
		if(SPBench::getBatchSize() > 1){
			if(item.batch_size >= SPBench::getBatchSize()) break;
		}
		
		if(SPBench::memory_source_is_enabled()){
			if(Metrics::items_counter < ret_in_memory_vector.size()){

				struct item_data *ret;
				item.item_batch.resize(item.batch_size+1);
				item.item_batch[item.batch_size] = ret_in_memory_vector[Metrics::items_counter];
			} else {
				stream_end = true;
				break;
			}
		} else {

			struct item_data* ret;
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
			item.item_batch.push_back(ret);
		}
		item.item_batch[item.batch_size]->index = Metrics::items_counter;
		item.batch_size++;
		Metrics::items_counter++;
	}

	//if this batch has size 0, ends computation
	if(item.batch_size == 0){
		return false;
	}

	//metrics computation
	if(Metrics::latency_is_enabled()){
		item.latency_op.push_back(current_time_usecs() - latency_op);
	}

	item.batch_index = Metrics::batch_counter;
	Metrics::batch_counter++;	// sent batches
	return true;
}

void Sink::op(Item &item){	
	//metrics computation
	unsigned long latency_op;
	if(Metrics::latency_is_enabled()){
		latency_op = current_time_usecs();
	}

	if(!SPBench::memory_source_is_enabled()){
		unsigned int num_item = 0;
		while(num_item < item.batch_size){ //batch loop

			struct item_data* ret;
			ret = item.item_batch[num_item];
			
			// Removing the full path of each query image from the output file
			// Removing the varying path allows to run correctness testing using md5 hash
			// Original form: fprintf(fout, "%s", ret->first.rank.name);
			std::string aux = ret->first.rank.name;
			std::size_t found = aux.find_last_of("//");
			fprintf(fout, "queries/%s", aux.substr(found+1).c_str());

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
			//free(ret);
		
			num_item++;
			Metrics::items_at_sink_counter++;
		}
	} else { // If in-memory is enabled, just count the items on the batch and proceed to the next batch
		Metrics::items_at_sink_counter += item.batch_size;
	}

	Metrics::batches_at_sink_counter++;

	if(Metrics::latency_is_enabled()){
		double current_time_sink = current_time_usecs();
		item.latency_op.push_back(current_time_sink - latency_op);

		unsigned long total_item_latency = (current_time_sink - item.timestamp);
		Metrics::global_latency_acc += total_item_latency; // to compute inst. average latency

		auto latency = Metrics::Latency_t();
		latency.local_latency = item.latency_op;
		latency.total_latency = total_item_latency;
		latency.item_timestamp = item.timestamp;
		latency.item_sink_timestamp = current_time_sink;
		latency.batch_size = item.batch_size;
		Metrics::latency_vector.push_back(latency);
		item.latency_op.clear();
	}
	if(Metrics::monitoring_is_enabled()){
		Metrics::monitor_metrics();
	}
}

} //end of namespace spb