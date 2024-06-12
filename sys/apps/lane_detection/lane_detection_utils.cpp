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

/**
 * Function to set the name of the operators
 * 
 * @return void
 */
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

/**
 * @brief Show the usage of the application
 * 
 * @param name of the application
 */
void usage(std::string name){
	fprintf(stderr, "Usage: %s\n", name.c_str());
	fprintf(stderr, "  -i, --input            <input_video> (mandatory)\n");
	printGeneralUsage();
	exit(-1);
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
	int opt = 0;
	int opt_index = 0;

	if(argc < 2) usage(argv[0]);

	try {
		//while ((opt = getopt(argc,argv,"i:t:b:B:m:M:F:u:p:klfxrh")) != EOF){
			while ((opt = getopt_long(argc, argv, "i:t:b:B:m:M:f:F:Il:L:Tru:h", long_opts, &opt_index)) != -1) {
			switch(opt){
				case 'i':
					input = optarg;
					if(!file_exists(input)) throw std::invalid_argument("\n ERROR in input --> Invalid video: " + input + "\n");
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
	
	if(Metrics::monitoring_thread_is_enabled()){
		Metrics::start_monitoring();
	}
}

/**
 * Function to end the benchmark
 *
 * @return void
 */
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
		
		if(SPBench::memory_source_is_enabled()){
			if(Metrics::items_at_source_counter < MemData.size()){
				item_data.image_p = &MemData[Metrics::items_at_source_counter];
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
		item_data.index = Metrics::items_at_source_counter;
		item.item_batch.resize(item.batch_size+1);
		item.item_batch[item.batch_size] = item_data;
		item.batch_size++;
		Metrics::items_at_source_counter++;
	}

	// if this batch has size 0, ends computation
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
			oVideoWriter.write(item.item_batch[num_item].image);
			item.item_batch[num_item].image.release();
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

} // end of namespace spb
