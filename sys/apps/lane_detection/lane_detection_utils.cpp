/**
 * ************************************************************************  
 *  File  : lane_detection_utils.cpp
 *
 *  Title : SPBench version of the Lane Detection
 *
 *  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 *
 *  Date  : July 06, 2021
 *
 * ************************************************************************
**/

/**
 * ------------------------------------------------------------------------------------------
 * Lane Detection:
 *
 * General idea and some code modified from:
 * chapter 7 of Computer Vision Programming using the OpenCV Library. 
 * by Robert Laganiere, Packt Publishing, 2011.
 * This program is free software; permission is hereby granted to use, copy, modify, 
 * and distribute this source code, or portions thereof, for any purpose, without fee, 
 * subject to the restriction that the copyright notice may not be removed 
 * or altered from any source or altered source distribution. 
 * The software is released on an as-is basis and without any warranties of any kind. 
 * In particular, the software is not guaranteed to be fault-tolerant or free from failure. 
 * The author disclaims all warranties with regard to this software, any use, 
 * and any consequent failure, is purely the responsibility of the user.
 *
 * Copyright (C) 2013 Jason Dorweiler, www.transistor.io
 * ------------------------------------------------------------------------------------------
 * Source:
 *
 * http://www.transistor.io/revisiting-lane-detection-using-opencv.html
 * https://github.com/jdorweiler/lane-detection
 * ------------------------------------------------------------------------------------------
 * Notes:
 * 
 * Add up number on lines that are found within a threshold of a given rho,theta and 
 * use that to determine a score.  Only lines with a good enough score are kept. 
 *
 * Calculation for the distance of the car from the center.  This should also determine
 * if the road in turning.  We might not want to be in the center of the road for a turn. 
 *
 * Several other parameters can be played with: min vote on houghp, line distance and gap.  Some
 * type of feed back loop might be good to self tune these parameters. 
 * 
 * We are still finding the Road, i.e. both left and right lanes.  we Need to set it up to find the
 * yellow divider line in the middle. 
 * 
 * Added filter on theta angle to reduce horizontal and vertical lines. 
 * 
 * Added image ROI to reduce false lines from things like trees/powerlines
 * ------------------------------------------------------------------------------------------
 */

#include <lane_detection_utils.hpp>

namespace spb{
bool stream_end = false;
std::vector<cv::Mat> MemData; //vector to store data in-memory

cv::VideoWriter oVideoWriter;
cv::VideoCapture capture;


void set_operators_name(){
	//vector<string> operator_name_list;
	SPBench::addOperatorName("Source ");
	SPBench::addOperatorName("Segment");
	SPBench::addOperatorName("Canny_1");
	SPBench::addOperatorName("HoughT ");
	SPBench::addOperatorName("HoughP ");
	SPBench::addOperatorName("Bitwise");
	SPBench::addOperatorName("Canny_2");
	SPBench::addOperatorName("Overlap");
	SPBench::addOperatorName("Sink   ");
	//return operator_name_list;
}

void usage(std::string name){
	fprintf(stderr, "Usage: %s\n", name.c_str());
	fprintf(stderr, "  -i, --input            <input_video> (mandatory)\n");
	printGeneralUsage();
	exit(-1);
}

void init_bench(int argc, char* argv[]){

	std::string input;
	int opt = 0;
	int opt_index = 0;

	if(argc < 2) usage(argv[0]);

	try {
		//while ((opt = getopt(argc,argv,"i:t:b:B:m:M:F:u:p:klfxrh")) != EOF){
			while ((opt = getopt_long(argc, argv, "i:t:b:B:m:M:f:F:IlLTru:h", long_opts, &opt_index)) != -1) {
			switch(opt){
				case 'i':
					input = optarg;
					if(!file_exists(input)) throw std::invalid_argument("\n ERROR in input --> Invalid video: " + input + "\n");
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
						throw std::invalid_argument("\n ARGUMENT ERROR (-f <frequency>) --> Frequency value must be higher than zero!\n");
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

	capture.open(input);

	//if this fails, try to open as a video camera, through the use of an integer param
	if (!capture.isOpened()) 
	{capture.open(atoi(input.c_str()));}

	//get the width of frames of the video
	double dWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	//get the height of frames of the video 
	double dHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT); 

	cv::Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));
	//initialize the VideoWriter object 
	oVideoWriter.open((prepareOutFileAt("outputs") + "_" + remove_extension(base_name(input)) + ".avi").c_str(), CV_FOURCC('P','I','M','1'), 20, frameSize, true);

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
	
	if(Metrics::monitoring_thread_is_enabled()){
		Metrics::start_monitoring();
	}
}

void end_bench(){

	if(SPBench::memory_source_is_enabled()){
		// for(Mat image : MemData){

		while(!MemData.empty()){
			oVideoWriter.write(MemData[0]);
			MemData.erase(MemData.begin());
		}

		if(!MemData.empty())
			MemData.erase(MemData.begin(), MemData.end());
	}
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
			if (item_data.image.empty()){
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

	// if this batch has size 0, ends computation
	if(item.batch_size == 0){
		return false;
	}

	if(Metrics::latency_is_enabled()){
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
			oVideoWriter.write(item.item_batch[num_item].image);
			item.item_batch[num_item].image.release();
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

} // end of namespace spb
