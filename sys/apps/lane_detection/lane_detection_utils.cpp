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
	fprintf(stderr, "\t-i\t: <input_file> (mandatory)\n");
	fprintf(stderr, "\t-b\t: <number_of_itens_per_batch>\n");
	fprintf(stderr, "\t-t\t: <number_of_threads>\n");
	fprintf(stderr, "\t-k\t: read entire input file into memory first\n");
	fprintf(stderr, "\t-l\t: print average latency results\n");
	fprintf(stderr, "\t-f\t: store individual latency values into a file\n");
	fprintf(stderr, "\t-m\t: monitors latency, throughput, and CPU and memory usage.\n");
	fprintf(stderr, "\t-x\t: print average throughput results\n");
	fprintf(stderr, "\t-u\t: print memory consumption results generated by UPL library\n");
	fprintf(stderr, "\t-h\t: print this help message\n");
	exit(-1);
}

void init_bench(int argc, char* argv[]){

	std::string input;
	int opt;
	while ((opt = getopt(argc,argv,"i:t:b:m:F:u:klfxrh")) != EOF){
		switch(opt){
			case 'i':
				input = optarg;
				break;
			case 't':
				nthreads = atoi(optarg);
				break;
			case 'b':
				SPBench::set_batch_size(atoi(optarg));
				break;
			case 'm':
				Metrics::set_monitoring_time_interval(atoi(optarg));
				Metrics::enable_monitoring();
				break;
			case 'F':
				SPBench::set_items_reading_frequency(atoi(optarg));
				SPBench::enable_memory_source();
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
	SPBench::bench_path = argv[0];
	if(!file_exists(input)){
		printf("Invalid input file! Try run: %s -i <input_file>\n", argv[0]);
		exit(1);
	}

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

bool Source::op(Item &item){

	// if last batch included the last item, ends computation
	if(stream_end == true){
		return false;
	}

	unsigned long latency_op;
	item.timestamp = current_time_usecs();
	if(Metrics::latency_is_enabled()){
		latency_op = current_time_usecs();
	}
	
	while(item.batch_size < SPBench::get_batch_size()){ //batch loop
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
	
	// frequency control mechanism
	SPBench::item_frequency_control(item.timestamp);

	if(Metrics::latency_is_enabled()){
		item.latency_op[0] = (current_time_usecs() - latency_op);
	}

	item.batch_index = Metrics::batch_counter;
	Metrics::batch_counter++;	// sent batches
	return true;
}

void Sink::op(Item &item){
	volatile unsigned long latency_op;
	if(Metrics::latency_is_enabled()){
		latency_op = current_time_usecs();
	}
	
	//when 'in-memory', do nothing here, the result is already ready on the output vector
	//if not in-memory, then retrieve the data from itens and write it on the disk
	if(!SPBench::memory_source_is_enabled()){                            
		unsigned int num_item = 0;
		while(num_item < item.batch_size){ //batch loop
			oVideoWriter.write(item.item_batch[num_item].image);
			num_item++;
		}
	}

	Metrics::items_at_sink_counter++;

	if(Metrics::monitoring_is_enabled()){
		Metrics::monitor_metrics(item.timestamp);
	}
	if(Metrics::latency_is_enabled()){
		item.latency_op[8] = (current_time_usecs() - latency_op);

		volatile unsigned long total_item_latency = (current_time_usecs() - item.timestamp);
		Metrics::global_latency_acc += total_item_latency; // to compute real time average latency
		Metrics::global_current_latency = total_item_latency; // to compute real time latency

		auto latency = Metrics::getLatency_t();
		latency.local_latency = item.latency_op;
		latency.local_total = total_item_latency;
		Metrics::latency_vector.push_back(latency);
		item.latency_op.clear();
	}
}

} // end of namespace spb
