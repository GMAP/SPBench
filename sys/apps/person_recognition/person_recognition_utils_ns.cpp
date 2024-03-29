/**
 * ************************************************************************  
 *  File  : person_recognition_utils_ns.cpp
 *
 *  Title : SPBench version of the nsources Person Recognition
 *
 *  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 *
 *  Date  : July 06, 2021
 *
 * ************************************************************************
**/

#include <person_recognition_utils_ns.hpp>

namespace spb{
std::vector<IO_data_struct> IO_data_vec;

void set_operators_name();
void read_training_set(const std::string &, std::vector<cv::Mat> &);
inline void usage(std::string);

void input_parser(char *);

void usage(std::string name){
	fprintf(stderr, "Usage: %s\n", name.c_str());
	fprintf(stderr, "\t-i\t: <input_string> (mandatory)\n");
	fprintf(stderr, "\t-b\t: <batch_size_in_number_of_items>\n");
	fprintf(stderr, "\t-B\t: <batch_size_in_milliseconds>\n");
	fprintf(stderr, "\t-f\t: <target_throughput>\n");
	fprintf(stderr, "\t-t\t: <number_of_threads>\n");
	fprintf(stderr, "\t-I\t: enable in-memory execution\n");
	fprintf(stderr, "\t-l\t: print average latency results\n");
	fprintf(stderr, "\t-L\t: store individual latency values into a log file\n");
	fprintf(stderr, "\t-m\t: monitors latency, throughput, and CPU and memory usage.\n");
	fprintf(stderr, "\t-T\t: print average throughput results\n");
	fprintf(stderr, "\t-r\t: print memory consumption results generated by UPL library\n");
	fprintf(stderr, "\t-u\t: send a custom argument to be used inside your programm\n");
	fprintf(stderr, "\t-h\t: print this help message\n");
	exit(-1);
}

void input_parser(char * argv){
	workload_data new_input;
	new_input.input_vid = strtok (argv," ");
	new_input.training_list = strtok (NULL," ");
	new_input.cascade_path = strtok (NULL," ");
	new_input.inputId = strtok(NULL," ");

	if(!file_exists(new_input.input_vid)){
		printf("Invalid input file!\n");
		//printf("Try run: ./app -i <input_file>\n", argv[0]);
		printf("You can use -h to see more options.\n");
		exit(1);
	}
	IO_data_vec.push_back({new_input});
}

void set_operators_name(){
	SPBench::addOperatorName("Source   ");
	SPBench::addOperatorName("Detect   ");
	SPBench::addOperatorName("Recognize");
	SPBench::addOperatorName("Sink     ");
}

void init_bench(int argc, char* argv[]){

	std::string input;
	int opt;
	while ((opt = getopt(argc,argv,"i:t:b:m:B:f:u:IlLTrh")) != EOF){
		switch(opt){
			case 'i':
				input_parser(optarg);
				break;
			case 't':
				nthreads = atoi(optarg);
				break;
			case 'b':
				if (atoi(optarg) <= 0)
					throw std::invalid_argument("\n ARGUMENT ERROR (-b <batch_size>) --> Batch size must be an integer value higher than zero!\n");
				SPBench::setBatchSize(atoi(optarg));
				break;
			case 'B':
				if (atof(optarg) <= 0.0)
					throw std::invalid_argument("\n ARGUMENT ERROR (-B <batch_interval>) --> Batch interval must be a value higher than zero!\n");
				SPBench::setBatchInterval(atof(optarg));
			case 'm':
				Metrics::set_monitoring_time_interval(atoi(optarg));
				Metrics::enable_monitoring();
				break;
			case 'f':
				if (atof(optarg) <= 0.0)
					throw std::invalid_argument("\n ARGUMENT ERROR (-F <frequency>) --> Frequency value must be higher than zero!\n");
				SPBench::setFrequency(atof(optarg));
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
	SPBench::bench_path = argv[0];
	set_operators_name();
	Metrics::enable_latency();
}

void Source::printStatus(){
	std::cout << "\n - New source added: " << IO_data_vec[sourceId].input_data.inputId << std::endl;

	std::cout << "\n - Workload:" << std::endl;
	std::cout << "        Input video: " << IO_data_vec[sourceId].input_data.input_vid << std::endl;
	std::cout << "       Cascade path: " << IO_data_vec[sourceId].input_data.cascade_path << std::endl;
	std::cout << "      Training list: " << IO_data_vec[sourceId].input_data.training_list << std::endl;

	std::cout << "\n - Setup:" << std::endl;
	std::cout << "         Batch size: " << sourceBatchSize << std::endl;
	std::cout << "     Batch interval: " << sourceBatchInterval << std::endl;
	
	if(sourceQueueSize == 0)
		std::cout << "    Queue max. size: 0 (unlimited)" << std::endl;
	else
		std::cout << "    Queue max. size: " << sourceQueueSize << std::endl;

	if(sourceFrequency == 0)
		std::cout << "    Input frequency: 0 (no limit)" << std::endl;
	else
		std::cout << "    Input frequency: " << sourceFrequency << " items per second" << std::endl;

	if(SPBench::memory_source_is_enabled())
		std::cout << "In-memory execution: enabled" << std::endl;
	std::cout << "\n###############################################" << std::endl;
}

void Source::source_op(){

	//VideoWriter oVideoWriter;
	cv::VideoCapture capture;

	cv::Ptr<cv::FaceRecognizer> model;
	cv::Size _faceSize;

	capture.open(IO_data_vec[sourceId].input_data.input_vid);

	cv::Size frame_size(static_cast<int>(capture.get(CV_CAP_PROP_FRAME_WIDTH)), static_cast<int>(capture.get(CV_CAP_PROP_FRAME_HEIGHT)));
	//fw.open(strcat(argv[0], "_result.avi"), OUT_FOURCC, OUT_FPS, frame_size, true);

	std::string output_file_name = (prepareOutFileAt("outputs") + "_" + IO_data_vec[sourceId].input_data.inputId + ".avi");

	IO_data_vec[sourceId].oVideoWriter.open(output_file_name.c_str(), OUT_FOURCC, OUT_FPS, frame_size, true);

	/** Initializations: **/
	std::vector<cv::Mat>  training_set;
	read_training_set(std::string(IO_data_vec[sourceId].input_data.training_list), training_set);
	//PersonRecognizer pr(training_set, LBPH_RADIUS, LBPH_NEIGHBORS, LBPH_GRID_X, LBPH_GRID_Y, LBPH_THRESHOLD);

	//all images are faces of the same person, so initialize the same label for all.
	std::vector<int> labels(training_set.size());
	for (std::vector<int>::iterator it = labels.begin(); it != labels.end(); *(it++) = 10);
	_faceSize = cv::Size(training_set[0].size().width, training_set[0].size().height);

	//build recognizer model:
	model = cv::createLBPHFaceRecognizer(LBPH_RADIUS, LBPH_NEIGHBORS, LBPH_GRID_X, LBPH_GRID_Y, LBPH_THRESHOLD);
	model->train(training_set, labels);
	
	//load the input to the memory before the stream region for in-memory execution
	if(SPBench::memory_source_is_enabled()){
		while(1){
			cv::Mat image;
			capture >> image;
			if (image.empty()) break;
			IO_data_vec[sourceId].MemData.push_back(image);
		}
	}

	IO_data_vec[sourceId].input_data.model = model;
	IO_data_vec[sourceId].input_data._faceSize = _faceSize;

	unsigned int sourceItemCounter = 0;
	bool end_of_input = false;

	while(1){
		
		Item item;

		//if last batch included the last item, ends computation
		if(end_of_input == true){
			item.setLastItem();
			sourceQueue.enqueue(item);
			break;
		}
		
		unsigned long latency_op;
		item.timestamp = current_time_usecs();
		if(Metrics::latency_is_enabled()){
			latency_op = current_time_usecs();
		}
		
		while(item.batch_size < sourceBatchSize){ //batch loop
		
			struct item_data item_data;
			item_data.sourceId = sourceId;

			if(SPBench::memory_source_is_enabled()){
				if(sourceItemCounter < IO_data_vec[sourceId].MemData.size()){
					item_data.image_p = &IO_data_vec[sourceId].MemData[sourceItemCounter];
				} else {
					end_of_input = true;
					break;
				}
			} else {
				capture >> item_data.image;
				if (item_data.image.empty()){
					end_of_input = true;
					break;
				}
			}
			
			item.item_batch.resize(item.batch_size+1);
			item_data.index = sourceItemCounter + 1;
			item.item_batch[item.batch_size] = item_data;
			item.batch_size++;
			item.setNotEmpty();
			sourceItemCounter++;
		}
		
		//if this batch has size 0, ends computation
		if(item.batch_size == 0){
			item.setLastItem();
			sourceQueue.enqueue(item);
			break;
		}
		
		// frequency control mechanism
		item_frequency_control(item.timestamp, sourceFrequency);

		if(Metrics::latency_is_enabled()){
			item.latency_op.push_back(current_time_usecs() - latency_op);
		}

		// put item in the output queue
		sourceQueue.enqueue(item);

		// Accumulate the total number of sent batches
		metrics_vec[sourceId].global_batch_counter++;

		// Update the total number of sent items
		metrics_vec[sourceId].global_item_counter = sourceItemCounter;
	}

	return;
}

void Sink::op(Item &item){

	if(item.empty())
		return;

	volatile unsigned long latency_op;
	if(Metrics::latency_is_enabled()){
		latency_op = current_time_usecs();
	}	
	
	// If in-memory is enabled, do nothing here, the result is already ready on the output vector
	// Else, then retrieve the data from items and write it on the disk
	if(!SPBench::memory_source_is_enabled()){
		unsigned int num_item = 0;
		while(num_item < item.batch_size){ //batch loop
			IO_data_vec[item.sourceId].oVideoWriter.write(item.item_batch[num_item].image);
			item.item_batch[num_item].image.release();	
			num_item++;
			metrics_vec[item.sourceId].items_at_sink_counter++;
		}
	} else { // If in-memory is enabled, just count the items on the batch and proceed to the next batch
		metrics_vec[item.sourceId].items_at_sink_counter += item.batch_size;
	}

	metrics_vec[item.sourceId].batches_at_sink_counter++;

	if(Metrics::monitoring_is_enabled()){
		monitor_metrics(item.timestamp, item.sourceId);
	}
	if(Metrics::latency_is_enabled()){
		double current_time_sink = current_time_usecs();
		item.latency_op.push_back(current_time_sink - latency_op);

		volatile unsigned long total_item_latency = (current_time_sink - item.timestamp);
		metrics_vec[item.sourceId].global_latency_acc += total_item_latency; // to compute real time average latency
		
		item_metrics_data latency;
		latency.local_latency = item.latency_op;
		latency.total_latency = total_item_latency;
		latency.item_sink_timestamp = current_time_sink;
		latency.batch_size = item.batch_size;
		metrics_vec[item.sourceId].latency_vector_ns.push_back(latency);
		metrics_vec[item.sourceId].stop_throughput_clock = current_time_sink;
		item.latency_op.clear();
	}
}

void end_bench(){
	if(SPBench::memory_source_is_enabled()){
		for (auto & element : IO_data_vec){

			while(!element.MemData.empty()){
				element.oVideoWriter.write(element.MemData[0]); // always write the first data element
				element.MemData.erase(element.MemData.begin()); // erase the writen data element from memory
			}

			if(!element.MemData.empty()) // erase any trash
				element.MemData.erase(element.MemData.begin(), element.MemData.end());
		}
	}
	compute_metrics();
}

void read_training_set(const std::string &list_path, std::vector<cv::Mat> &images) {
	std::ifstream file(list_path.c_str());
	std::string path;
	while (getline(file, path)) {
		images.push_back(cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE));
	}
}

} //end of namespace spb