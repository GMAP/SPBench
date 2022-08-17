/**
 * ************************************************************************  
 *  File  : person_recognition_utils.cpp
 *
 *  Title : SPBench version of the Person Recognition
 *
 *  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 *
 *  Date  : July 06, 2021
 *
 * ************************************************************************
**/

#include <person_recognition_utils.hpp>

namespace spb{

//Globals:
cv::VideoCapture capture;
cv::VideoWriter fw;

cv::Ptr<cv::FaceRecognizer> model;
cv::Size _faceSize;

std::vector<cv::Mat> MemData; //vector to store data in-memory

workload_data input_data;

int Source::operator_id = -1;
int Sink::operator_id = -1;

bool stream_end = false;

void set_operators_name();
void read_training_set(const std::string &, std::vector<cv::Mat> &);
inline void usage(std::string);

void input_parser(char *);

inline void usage(std::string name){
	fprintf(stderr, "Usage: %s\n", name.c_str());
	fprintf(stderr, "\t-i\t: <input_video, training_list, cascade_path> (mandatory)\n");
	fprintf(stderr, "\t-b\t: <batch_size_in_number_of_items>\n");
	fprintf(stderr, "\t-B\t: <batch_size_in_milliseconds>\n");
	fprintf(stderr, "\t-F\t: <target_throughput>\n");
	fprintf(stderr, "\t-p\t: Frequency pattern: <pattern,period,min,max>\n");
	fprintf(stderr, "\t-t\t: <number_of_threads>\n");
	fprintf(stderr, "\t-k\t: enable in-memory execution\n");
	fprintf(stderr, "\t-l\t: print average latency results\n");
	fprintf(stderr, "\t-f\t: store individual latency values into a log file\n");
	fprintf(stderr, "\t-m\t: monitors latency, throughput, and CPU and memory usage.\n");
	fprintf(stderr, "\t-x\t: print average throughput results\n");
	fprintf(stderr, "\t-r\t: print memory consumption results generated by UPL library\n");
	fprintf(stderr, "\t-u\t: send a custom argument to be used inside your programm\n");
	fprintf(stderr, "\t-h\t: print this help message\n");
	exit(-1);
}

void input_parser(char * input){

	input_data.input_vid = split_string(input, ' ')[0];
	input_data.training_list = split_string(input, ' ')[1];
	input_data.cascade_path = split_string(input, ' ')[2];

	if(split_string(input, ' ').size() == 4)
		input_data.id = split_string(input, ' ')[3];
	else
		input_data.id = split_string(input, ' ')[0];

	if(!file_exists(input_data.input_vid)) throw std::invalid_argument("\n ERROR in input --> Invalid video: " + split_string(input, ' ')[0] + "\n");
	if(!file_exists(input_data.training_list)) throw std::invalid_argument("\n ERROR in input --> Invalid training list: " + split_string(input, ' ')[1] + "\n");
	if(!file_exists(input_data.cascade_path)) throw std::invalid_argument("\n ERROR in input --> Invalid cascade path: " + split_string(input, ' ')[2] + "\n");
}

void init_bench(int argc, char* argv[]){

	std::string input;
	int opt;

	std::string pattern;

	try {
		while ((opt = getopt(argc,argv,"i:t:b:B:m:F:u:p:klfxrh")) != EOF){
			switch(opt){
				case 'i':
					if(split_string(optarg, ' ').size() < 3) 
						throw std::invalid_argument("\n ARGUMENT ERROR --> Invalid input. Required: -i <input_video> <training_list> <cascade_path> <id (optional)>\n");
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
					Metrics::set_monitoring_time_interval(atoi(optarg));
					Metrics::enable_monitoring();
					break;
				case 'F':
					if (atof(optarg) <= 0.0)
						throw std::invalid_argument("\n ARGUMENT ERROR (-F <frequency>) --> Frequency value must be a positive value higher than zero!\n");
					SPBench::setFrequency(atof(optarg));
					break;
				case 'p':
					pattern = optarg;
					input_freq_pattern_parser(pattern);
					break;
				case 'k':
					SPBench::enable_memory_source();
					break;
				case 'l':
					Metrics::enable_print_latency();
					break;
				case 'f':
					Metrics::enable_latency_to_file();
					break;
				case 'x':
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

	capture.open(input_data.input_vid);

	cv::Size frame_size(static_cast<int>(capture.get(CV_CAP_PROP_FRAME_WIDTH)), static_cast<int>(capture.get(CV_CAP_PROP_FRAME_HEIGHT)));
	//fw.open(strcat(argv[0], "_result.avi"), OUT_FOURCC, OUT_FPS, frame_size, true);
	
	std::string output_file_name = (prepareOutFileAt("outputs") + "_" + input_data.id + ".avi");

	fw.open(output_file_name.c_str(), OUT_FOURCC, OUT_FPS, frame_size, true);

	//fw.open((out_file_path("outputs") + ".avi").c_str(), OUT_FOURCC, OUT_FPS, frame_size, true);
	/** Initializations: **/
	std::vector<cv::Mat> training_set;
	//frameId = 0;
	
	read_training_set(std::string(input_data.training_list), training_set);
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
			MemData.push_back(image);
		}
	}

	set_operators_name();
	Metrics::enable_latency();
}

void set_operators_name(){
	SPBench::addOperatorName("Source   ");
	SPBench::addOperatorName("Detect   ");
	SPBench::addOperatorName("Recognize");
	SPBench::addOperatorName("Sink     ");
}

long Source::source_item_timestamp = current_time_usecs();

bool Source::op(Item &item){

	if(operator_id < 0){
		operator_id = SPBench::getNewOpId();
	}

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
			if(((current_time_usecs() - batch_elapsed_time) / 1000.0) >= SPBench::getBatchInterval()) break;
		} else {
			if(item.batch_size >= SPBench::getBatchSize()) break;
		}
		if(SPBench::getBatchSize() > 1){
			if(item.batch_size >= SPBench::getBatchSize()) break;
		}

		item_data item_data;

		if(SPBench::memory_source_is_enabled()){
			if(Metrics::items_counter < MemData.size()){
				item_data.image_p = &MemData[Metrics::items_counter];
			} else {
				stream_end = true;
				break;
			}
		} else {
			capture >> item_data.image;
			if(item_data.image.empty()){
				stream_end = true;
				break;
			}
		}

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
		item.latency_op[operator_id] = (current_time_usecs() - latency_op) ;
	}

	item.batch_index = Metrics::batch_counter;
	Metrics::batch_counter++;	// sent batches
	return true;
}

void Sink::op(Item &item){
	
	if(operator_id < 0){
		operator_id = SPBench::getNewOpId();
	}
	
	unsigned long latency_op;
	if(Metrics::latency_is_enabled()){
		latency_op = current_time_usecs();
	}	
	
	//when 'in-memory', do nothing here, the result is already ready on the output vector
	//if not in-memory, then retrieve the data from itens and write it on the disk
	if(!SPBench::memory_source_is_enabled()){
		unsigned int num_item = 0;
		while(num_item < item.batch_size){ //batch loop
			fw.write(item.item_batch[num_item].image);
			item.item_batch[num_item].image.release();	
			num_item++;
			Metrics::items_at_sink_counter++;
		}
	}
	
	Metrics::batches_at_sink_counter++;

	if(Metrics::latency_is_enabled()){
		double current_time_sink = current_time_usecs();
		item.latency_op[operator_id] = (current_time_sink - latency_op);

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
		Metrics::last_batch_size = item.batch_size;
		Metrics::monitor_metrics();
	}
}

void end_bench(){
	if(SPBench::memory_source_is_enabled()){
		while(!MemData.empty()){
			fw.write(MemData[0]);
			MemData.erase(MemData.begin());
		}
		if(!MemData.empty())
			MemData.erase(MemData.begin(), MemData.end());
	}
}

void read_training_set(const std::string &list_path, std::vector<cv::Mat> &images) {
	std::ifstream file(list_path.c_str());
	std::string path;
	while (getline(file, path)) {
		images.push_back(cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE));
	}
}

} //end of namespace spb