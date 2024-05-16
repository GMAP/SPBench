/**
 * ************************************************************************  
 *        File : fraud_detection_utils.cpp
 *
 *       Title : SPBench version of the Fraud Detection
 * 
 * Description : This file contains the main functions of the 
 *               Fraud Detection application
 *
 *      Author : Adriano Marques Garcia <adriano1mg@gmail.com> 
 *
 *        Date : December 20, 2022
 *
 * ************************************************************************
**/

#include <fraud_detection_utils.hpp>

namespace spb{

//Markov_Model_Predictor predictor;

//Globals:

using count_key_t = std::pair<size_t, uint64_t>; // contains a pair of entity_id and a counter for each entity_id
using key_map_t = std::unordered_map<std::string, count_key_t>; // contains a mapping between string keys and integer keys for each entity_id

key_map_t entity_key_map; // contains a mapping between string keys and integer keys for each entity_id
size_t entity_unique_key = 0; // unique integer key
std::vector<std::pair<std::string, std::string>> parsed_file; // contains strings extracted from the input file

size_t next_tuple_idx = 0;

float iteractions;

std::string input_file, input_id;

std::vector<spb::Item> InMemData; //vector to store data in-memory

#if !defined NO_OUTPUT_FILE
	std::ofstream outputFile;
	std::vector<spb::Item> OutMemData; //vector to store data in-memory
#endif

bool stream_end = false;

void set_operators_name();
inline void usage(std::string);
void map_and_parse_dataset();
void input_parser(char *);

/**
 * Usage of this specific application
 * 
 * @param name name of the application
 * @return void
 */
void usage(std::string name){
	fprintf(stderr, "Usage: %s\n", name.c_str());
	fprintf(stderr, "  -i, --input            \"<dataset> <dataset_iteractions> <input_id (optional)>\" (mandatory)\n");
	printGeneralUsage();
	exit(-1);
}

/**
 * Function to parse the input string
 * 
 * @param input input string
 * @return void
 */
void input_parser(char * input){

	input_file = split_string(input, ' ')[0];
	iteractions = stof(split_string(input, ' ')[1]);

	if(split_string(input, ' ').size() == 3)
		input_id = split_string(input, ' ')[2];
	else
		input_id = split_string(input, ' ')[1];

	if(!file_exists(input_file)) throw std::invalid_argument("\n ERROR --> Input file not found: " + split_string(input, ' ')[0] + "\n");

	if(!(iteractions > 0)) throw std::invalid_argument("\n ERROR --> Dataset iteractions value must be higher than zero: " + split_string(input, ' ')[0] + "\n");
}

/**
 * Mapping of the dataset
 * 
 * @return void
 */
void map_and_parse_dataset(){
	std::string split_regex = ",";
    std::ifstream file(input_file);
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            //printf("Get line\n");
            std::string entity_id = line.substr(0, line.find(split_regex));
            std::string record = line.substr(line.find(split_regex) + 1, line.size());
            // map keys
            if (entity_key_map.find(entity_id) == entity_key_map.end()) { // the key is not present in the hash map
                entity_key_map.insert(std::make_pair(entity_id, std::make_pair(entity_unique_key, 0)));
                entity_unique_key++;
            }
            // save parsed file
            parsed_file.insert(parsed_file.end(), make_pair(entity_id, record));
        }
        file.close();
    }
}

/**
 * Function to initialize the benchmark
 *
 * @param argc number of arguments
 * @param argv arguments
 * @return void
 */
void init_bench(int argc, char* argv[]){

	int opt;
	int opt_index = 0;

	if(argc < 2) usage(argv[0]);

	try {
		//while ((opt = getopt(argc,argv,"i:t:b:B:m:M:F:u:p:klfxrh")) != EOF){
		while((opt = getopt_long(argc, argv, "i:t:b:B:m:M:f:F:Il:L:Tru:h", long_opts, &opt_index)) != -1) {
			switch(opt){
				case 'i':
					if(split_string(optarg, ' ').size() < 2) 
						throw std::invalid_argument("\n ARGUMENT ERROR --> Invalid input. Required: -i \"<dataset> <dataset_iteractions> <input_id (optional)>\"\n");
					input_parser(optarg);
					break;
				case 't':
				case 'm':
				case 'M':
				case 'f':
				case 'F':
				case 'I':
				case 'l':
				case 'L':
				case 'T':
				case 'r':
				case 'u':
					// all the above empty cases fall into this option
					SPBench::parseCMDLine(opt, optarg);
					break;
				case 'b':
				case 'B':
					throw std::invalid_argument("BATCH OPTION ERROR\n SPBench still does not provide native batching mechanisms with key-by data partitioning support, wich is a requirement of this application. \n You can still use batching in SPBench with the acceleration library of your choice.");
				case 'h':
					std::cout << std::endl; 
					usage(argv[0]);
					break;
				default: /* '?' */
					std::cout << std::endl; 
					usage(argv[0]);
					exit(EXIT_FAILURE);
			}
		}
	} catch(const std::invalid_argument& e) {
		std::cerr << "exception: " << e.what() << std::endl;
		printf(" You can use -h to see more options.\n");
		exit(1);
	} catch (const char* msg) {
		std::cerr << "exception: " << msg << std::endl;
		printf(" You can use -h to see more options.\n");
		exit(1);
	} catch(...) {
		std::cout << std::endl; 
		exit(1);
	}

	SPBench::bench_path = argv[0];

	map_and_parse_dataset(); //chama as funções para a computação

	#if !defined NO_OUTPUT_FILE
		std::string output_file_name = (prepareOutFileAt("outputs") + "_" + input_id + ".txt");
		outputFile.open(output_file_name);
	#endif
	
	//load the input to the memory before the stream region for in-memory execution
	if(SPBench::memory_source_is_enabled()){
		//std::uniform_int_distribution<std::mt19937::result_type> dist(0, num_keys-1);
		//mt19937 rng;
		//rng.seed(0);
		for (int next_tuple_idx = 0; next_tuple_idx < parsed_file.size(); next_tuple_idx++) {
			// create tuple
			auto tuple_content = parsed_file.at(next_tuple_idx);
			Item item;
			item.record = tuple_content.second;
			//if (num_keys == 0) {
			item.key = (entity_key_map.find(tuple_content.first)->second).first;
			//}
			//else {
			//    t.key = dist(rng);
			//}
			// t.ts = 0L;
			InMemData.push_back(item);
		}
	}

	set_operators_name();
	//Metrics::enable_latency();

	if(Metrics::monitoring_thread_is_enabled()){
		Metrics::start_monitoring();
	}
}

/**
 * Function to set the name of the operators
 * 
 * @return void
 */
void set_operators_name(){
	SPBench::addOperatorName("Source   ");
	SPBench::addOperatorName("Predictor");
	SPBench::addOperatorName("Sink     ");
}

std::chrono::high_resolution_clock::time_point Source::source_item_timestamp = std::chrono::high_resolution_clock::now();

/**
 * Function to execute the source operator
 * 
 * @param item item to be processed
 * @return bool
 */
bool Source::op(Item &item){

	//if last batch included the last item, ends computation
	//if(stream_end == true){
	//	return false;
	//}

	// frequency control mechanism
	SPBench::item_frequency_control(source_item_timestamp);

	item.timestamp = source_item_timestamp = std::chrono::high_resolution_clock::now();
	//unsigned long batch_elapsed_time = source_item_timestamp;
	
	//unsigned long op_timestamp1;
	std::chrono::high_resolution_clock::time_point op_timestamp1;
	if(Metrics::latency_is_enabled()){
  		op_timestamp1 = item.timestamp;
	}

	//while(1) { //main source loop
		// batching management routines
		/*if(SPBench::getBatchInterval()){
			// Check if the interval of this batch is higher than the batch interval defined by the user
			if(((current_time_usecs() - batch_elapsed_time) / 1000.0) >= SPBench::getBatchInterval()) break;
		} else {
			// If no batch interval is set, than try to close it by size
			if(item.batch_size >= SPBench::getBatchSize()) break;
		}
		// This couples with batching interval to close the batch by size if a size higher than one is defined
		if(SPBench::getBatchSize() > 1){
			if(item.batch_size >= SPBench::getBatchSize()) break;
		}*/

		//if(Metrics::items_at_source_counter >= parsed_file.size() * iteractions){
		//	stream_end = true;
		//	return false;
			//break;
		//}

	std::chrono::duration<float> source_elapsed_time = std::chrono::duration_cast<std::chrono::duration<float>>(item.timestamp - Metrics::metrics.start_throughput_clock);

	if(source_elapsed_time.count() >= iteractions){
		return false;
	}

	if(SPBench::memory_source_is_enabled()){
		item.record = InMemData.at(next_tuple_idx).record;
		item.key = InMemData.at(next_tuple_idx).key;
	} else {
		// create tuple		
		auto tuple_content = parsed_file.at(next_tuple_idx);
		item.record = tuple_content.second;
		item.key = (entity_key_map.find(tuple_content.first)->second).first;
	}
	next_tuple_idx = (next_tuple_idx + 1) % parsed_file.size();	

//	item.index = Metrics::items_at_source_counter;
	//item.item_batch.resize(item.batch_size+1);
	//item.item_batch[item.batch_size] = item_data;
	item.batch_size++;
	//Metrics::items_at_source_counter++;

	//}

	//if this batch has size 0, ends computation
	//if(item.batch_size == 0){
	//	return false;
	//}

	if(Metrics::latency_is_enabled()){
		//item.latency_op.push_back(current_time_usecs() - latency_op);
		std::chrono::high_resolution_clock::time_point op_timestamp2 = std::chrono::high_resolution_clock::now();
		item.latency_op.push_back(std::chrono::duration_cast<std::chrono::duration<float>>(op_timestamp2 - op_timestamp1));
	}

	//item.batch_index = Metrics::batch_counter;
	//Metrics::batch_counter++;	// sent batches
	Metrics::items_at_source_counter++;
	return true;
}

/**
 * Function to execute the sink operator
 * 
 * @param item item to be processed
 * @return void
 */
void Sink::op(Item &item){
	
	if(item.record.empty()) return;

	#if !defined NO_LATENCY
		std::chrono::high_resolution_clock::time_point op_timestamp1;
		if(Metrics::latency_is_enabled()){
			op_timestamp1 = std::chrono::high_resolution_clock::now();
		}
	#endif

	//when 'in-memory', do nothing here, the result is already ready on the output vector
	//if not in-memory, then retrieve the data from itens and write it on the disk
	//unsigned int num_item = 0;
	//while(num_item < item.batch_size){ //batch loop
	#if !defined NO_OUTPUT_FILE
		if(!SPBench::memory_source_is_enabled()){
			outputFile << item.record << " " << item.key << " " << item.score << std::endl;
		} else {
			OutMemData.push_back(item);
		}
	#endif		

	
		//if(item.isOutlier) {
		//	Metrics::items_at_sink_counter++;
		//}
		
	//	num_item++;
	Metrics::items_at_sink_counter++;
	//} 
	
	Metrics::batches_at_sink_counter++;

	#if !defined NO_LATENCY
		if(Metrics::latency_is_enabled()){
			std::chrono::high_resolution_clock::time_point op_timestamp2 = std::chrono::high_resolution_clock::now();

			std::chrono::duration<float, std::milli> elapsed_time_since_last_sample = op_timestamp2 - Metrics::latency_last_sample_time;

			if(elapsed_time_since_last_sample.count() >= Metrics::latency_sample_interval){

				Metrics::latency_last_sample_time = op_timestamp2;

				item.latency_op.push_back(std::chrono::duration_cast<std::chrono::duration<float>>(op_timestamp2 - op_timestamp1));

				std::chrono::duration<float, std::milli> total_item_latency = op_timestamp2 - item.timestamp;

				if(total_item_latency.count() > 0.0){
					Metrics::global_latency_acc.total += total_item_latency; // to compute real time average latency
					Metrics::global_latency_acc.count++;

					auto latency = Metrics::Latency_t();
					latency.local_latency = item.latency_op;
					latency.total_latency = total_item_latency;
					latency.item_timestamp = item.timestamp;
					latency.item_sink_timestamp = op_timestamp2;
					latency.batch_size = item.batch_size;
					Metrics::latency_vector.push_back(latency);
					item.latency_op.clear();
				}
			}
		}
	#endif

	if(Metrics::monitoring_is_enabled()){
		Metrics::monitor_metrics();
	}
}

/**
 * Function to finalize the execution of the benchmark
 * 
 * @return void
 */
void end_bench(){

	if(SPBench::memory_source_is_enabled()){
		// make sure InMemData will be clean
		if(!InMemData.empty())
			InMemData.erase(InMemData.begin(), InMemData.end());

		#if !defined NO_OUTPUT_FILE
			std::cout << " Writing result to the output file... ";
			fflush(stdout);

			// Write results and clean output vector
			while(!OutMemData.empty()){
				
				outputFile << OutMemData.at(0).record << " " << OutMemData.at(0).key << " " << OutMemData.at(0).score << std::endl;
				OutMemData.erase(OutMemData.begin());
			}
			if(!OutMemData.empty())
				OutMemData.erase(InMemData.begin(), OutMemData.end());

			std::cout << "done!" << std::endl;
			fflush(stdout);
		#endif
	}
	#if !defined NO_OUTPUT_FILE	
		outputFile.close();
	#endif

	std::cout << " --> Potential frauds (temp.): " << Metrics::items_at_sink_counter << std::endl;
	
}

} //end of namespace spb