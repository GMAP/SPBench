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

void usage(std::string name){
	fprintf(stderr, "Usage: %s\n", name.c_str());
	fprintf(stderr, "  -i, --input            \"<some_string> <some_integer> ... \" (mandatory)\n");
	printGeneralUsage();
	exit(-1);
}

void input_parser(char * input){

	input_data.some_input_file = split_string(input, ' ')[0];
	input_data.some_integer = stoi(split_string(input, ' ')[1]);

	if(split_string(input, ' ').size() == 3)
		input_data.id = split_string(input, ' ')[2];
	else
		input_data.id = split_string(input, ' ')[0];

	//if(!file_exists(input_data.some_input_file)) throw std::invalid_argument("\n ERROR in input --> Invalid string: " + split_string(input, ' ')[0] + "\n");
}

void init_bench(int argc, char* argv[]){

	std::string input;
	int opt;
	int opt_index = 0;

	//std::string pattern;

	if(argc < 2) usage(argv[0]);

	try {
		//while ((opt = getopt(argc,argv,"i:t:b:B:m:M:F:u:p:klfxrh")) != EOF){
		while ((opt = getopt_long(argc, argv, "i:t:b:B:m:M:f:F:IlLTru:h", long_opts, &opt_index)) != -1) {
			switch(opt){
				case 'i':
					std::cout << optarg << std::endl;
					if(split_string(optarg, ' ').size() < 3) 
						throw std::invalid_argument("\n ARGUMENT ERROR --> Invalid input: Required: -i <some_string> <some_integer> <input id (optional)>\n");
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
					//pattern = optarg;
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
					std::cout << std::endl; 
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
	Metrics::enable_latency();

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

		item_data item_data;

		/* ****************************************** */
		/* ****** YOUR CODE GOES HERE (SOURCE) ****** */
		/* ⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄⌄ */

		// Example of how to read the data from the input file, including the support for in-memory option:
		if(SPBench::memory_source_is_enabled()){
			if(Metrics::items_counter < MemData.size()){
				item_data.some_pointer = &MemData[Metrics::items_counter];
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

		item_data.index = Metrics::items_counter;
		item.item_batch.resize(item.batch_size+1);
		item.item_batch[item.batch_size] = item_data;
		item.batch_size++;
		Metrics::items_counter++;
	}

	//if this batch has size 0, ends computation
	if(item.batch_size == 0){
		return false;
	}

	if(Metrics::latency_is_enabled()){
		//item.latency_op.push_back(current_time_usecs() - latency_op);
		item.latency_op.push_back(current_time_usecs() - latency_op);
	}

	item.batch_index = Metrics::batch_counter;
	Metrics::batch_counter++;	// sent batches
	return true;
}

void Sink::op(Item &item){
	
	unsigned long latency_op;
	if(Metrics::latency_is_enabled()){
		latency_op = current_time_usecs();
	}	
	
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

	if(Metrics::latency_is_enabled()){
		double current_time_sink = current_time_usecs();
		item.latency_op.push_back(current_time_sink - latency_op);

		unsigned long total_item_latency = (current_time_sink - item.timestamp);
		Metrics::global_latency_acc += total_item_latency; // to compute real time average latency

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