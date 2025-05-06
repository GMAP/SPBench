#include <new_app_utils.hpp>
namespace spb{

//Globals:
std::ifstream inputFile;
std::ofstream outputFile;

std::vector<std::string> MemData; //vector to store data in-memory

workload_data input_data;

int some_external_variable = 2;

bool stream_end = false;

void set_operators_name();
inline void usage(std::string);

void input_parser(char *);

/**
 * @brief Show the usage of the application
 * 
 * @param name of the application
 */
void usage(std::string name){
	fprintf(stderr, "Usage: %s\n", name.c_str());
	fprintf(stderr, "  -i, --input            \"<some_string> <some_integer> ... \" (mandatory)\n");
	printGeneralUsage();
	exit(-1);
}

/**
 * Function to parse the input string
 *
 * @param input string to be parsed
 * @return void
 */
void input_parser(char * input){

	input_data.some_input_file = split_string(input, ' ')[0];
	input_data.some_integer = stoi(split_string(input, ' ')[1]);

	if(split_string(input, ' ').size() == 3)
		input_data.id = split_string(input, ' ')[2];
	else
		input_data.id = split_string(input, ' ')[0];
}

/**
 * Function to initialize the benchmark
 *
 * @param argc number of arguments
 * @param argv arguments
 * @return void
 */
void init_bench(int argc, char* argv[]){

	std::string input;
	int opt;
	int opt_index = 0;

	if(argc < 2) usage(argv[0]);

	try {
		while ((opt = getopt_long(argc, argv, "i:t:b:B:m:M:f:F:Il:L:Tru:h", long_opts, &opt_index)) != -1) {
			switch(opt){
				case 'i':
					/* ****************************************** */
					/* **** YOUR CODE GOES HERE (INIT_BENCH) **** */
					/* ⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄ */
					
					// Input for each application are handle differently, so you have to adapt this part for your application
					std::cout << optarg << std::endl;
					if(split_string(optarg, ' ').size() < 2) 
						throw std::invalid_argument("\n ARGUMENT ERROR --> Invalid input: Required: -i <some_string> <some_integer> <input id (optional)>\n");
					input_parser(optarg);

					/* ⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃ */
					/* ****************************************** */
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
				case 'b':
				case 'B':
				case 'u':
					// all the above empty cases fall into this option
					if(optarg != NULL) {
						SPBench::parseCMDLine(opt, optarg);
					} else {
						SPBench::parseCMDLine(opt, "");
					}
					break;
				case 'h':
					usage(argv[0]);
					break;
				case '?': 
					usage(argv[0]);
					break;
				default: 
					std::cout << std::endl; 
					exit(1);
			}
		}
	} catch(const std::invalid_argument& e) {
		std::cerr << "exception: " << e.what() << std::endl;
		exit(1);
	} catch (const char* msg) {
		std::cerr << "exception: " << msg << std::endl;
		exit(1);
	} catch(...) {
		exit(1);
	}

	SPBench::bench_path = argv[0];

	/* ****************************************** */
	/* **** YOUR CODE GOES HERE (INIT_BENCH) **** */
	/* ⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄ */

	// Example of how to open the input and output files:
	inputFile.open(input_data.some_input_file);
	if(!inputFile.is_open()) throw std::invalid_argument("\n ERROR in input --> Invalid input file: " + input_data.some_input_file + "\n");

	std::string output_file_name = (prepareOutFileAt("outputs") + "_" + input_data.id + ".extension");
	outputFile.open(output_file_name);
	
	// load the input to the memory before the stream region for in-memory execution, which is optional
	if(SPBench::memory_source_is_enabled()){
		while(1){
			std::string data;
			if (!getline(inputFile, data)) break;
			MemData.push_back(data);
		}
		inputFile.close();
	}

	/* ⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃ */
	/* ****************************************** */

	set_operators_name();

	if(Metrics::monitoring_thread_is_enabled()){
		Metrics::start_monitoring();
	}
}

// Starting to set the operators name
void set_operators_name(){
	SPBench::addOperatorName("Source     ");
	SPBench::addOperatorName("Operator1  ");
	SPBench::addOperatorName("Operator2  ");
	SPBench::addOperatorName("Sink       ");
}
// End of setting the operators name

// Source clock initialization
std::chrono::high_resolution_clock::time_point Source::source_item_timestamp = std::chrono::high_resolution_clock::now();

/**
 * Function to execute the source operator
 *
 * @param item item to be processed
 * @return bool
 */
bool Source::op(Item &item){

	//if last batch included the last item, ends computation
	if(stream_end == true){
		return false;
	}

	// frequency control mechanism
	SPBench::item_frequency_control(source_item_timestamp);

	std::chrono::high_resolution_clock::time_point batch_opening_time = item.timestamp = source_item_timestamp = std::chrono::high_resolution_clock::now();
	
	#if !defined NO_LATENCY
		std::chrono::high_resolution_clock::time_point op_timestamp1;
		if(Metrics::latency_is_enabled()){
  			op_timestamp1 = item.timestamp;
		}
	#endif

	while(1) { //main source loop
		// batching management routines
		if(SPBench::getBatchInterval()){
			// Check if the interval of this batch is higher than the batch interval defined by the user
			std::chrono::duration<float, std::milli> batch_elapsed_time_ms = std::chrono::high_resolution_clock::now() - batch_opening_time;
			if(batch_elapsed_time_ms.count() >= SPBench::getBatchInterval()) break;
		} else {
			// If no batch interval is set, than try to close it by size
			if(item.batch_size >= SPBench::getBatchSize()) break;
		}
		// This couples with batching interval to close the batch by size if a size higher than one is defined
		if(SPBench::getBatchSize() > 1){
			if(item.batch_size >= SPBench::getBatchSize()) break;
		}
		item_data item_data;

		/* ****************************************** */
		/* ****** YOUR CODE GOES HERE (SOURCE) ****** */
		/* ⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄ */

		// Example of how to read the data from the input file, including the support for in-memory option:
		if(SPBench::memory_source_is_enabled()){
			if(Metrics::items_at_source_counter < MemData.size()){
				item_data.some_pointer = &MemData[Metrics::items_at_source_counter];
			} else {
				stream_end = true;
				break;
			}
		} else {
			std::string data;
			if (!getline(inputFile, data)){
				stream_end = true;
				break;
			}
			item_data.some_string = data;
		}

		/* ⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃ */
		/* ****************************************** */

		item_data.index = Metrics::items_at_source_counter;
		item.item_batch.resize(item.batch_size+1);
		item.item_batch[item.batch_size] = item_data;
		item.batch_size++;
		Metrics::items_at_source_counter++;
	}

	//if this batch has size 0, ends computation
	if(item.batch_size == 0){
		return false;
	}

	#if !defined NO_LATENCY
		if(Metrics::latency_is_enabled()){
			std::chrono::high_resolution_clock::time_point op_timestamp2 = std::chrono::high_resolution_clock::now();
			item.latency_op.push_back(std::chrono::duration_cast<std::chrono::duration<float>>(op_timestamp2 - op_timestamp1));
		}
	#endif

	item.batch_index = Metrics::batch_counter;
	Metrics::batch_counter++;	// sent batches

	return true;
}

/**
 * Function to execute the sink operator
 *
 * @param item item to be processed
 * @return void
 */
void Sink::op(Item &item){
	
	//metrics computation
	#if !defined NO_LATENCY
		std::chrono::high_resolution_clock::time_point op_timestamp1;
		if(Metrics::latency_is_enabled()){
			op_timestamp1 = std::chrono::high_resolution_clock::now();
		}
	#endif
	
	//when 'in-memory', do nothing here, the result is already ready on the output vector
	//if not in-memory, then retrieve the data from itens and write it on the disk
	if(!SPBench::memory_source_is_enabled()){
		unsigned int num_item = 0;
		while(num_item < item.batch_size){ //batch loop
			/* ****************************************** */
			/* ******* YOUR CODE GOES HERE (SINK) ******* */
			/* ⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄ */

			// Example of how to write the data to the output file:
			outputFile << item.item_batch[num_item].some_vector.back() << std::endl;

			/* ⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃ */
			/* ****************************************** */

			num_item++;
			Metrics::items_at_sink_counter++;
		}
	} else { // If in-memory is enabled, just count the items on the batch and proceed to the next batch
		Metrics::items_at_sink_counter += item.batch_size;
	}
	
	Metrics::batches_at_sink_counter++;

	//metrics computation
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
 * Function to end the benchmark
 *
 * @return void
 */
void end_bench(){
	/* ******************************************* */
	/* ***** YOUR CODE GOES HERE (END BENCH)  **** */
	/* ⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄ */

	if(SPBench::memory_source_is_enabled()){
		while(!MemData.empty()){
			outputFile << MemData[0];
			MemData.erase(MemData.begin());
		}
		if(!MemData.empty())
			MemData.erase(MemData.begin(), MemData.end());
	}
	if(inputFile.is_open()) inputFile.close();
	if(outputFile.is_open()) outputFile.close();

	/* ⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃⌃ */
	/* ******************************************* */
}

} //end of namespace spb