/**
 * ************************************************************************  
 *  File  : lane_detection.hpp
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

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <chrono>
#include "linefinder.h"
#include <sys/time.h>

#define FRAME_ID_INITIALIZATION -1

using namespace cv;
using namespace std;

VideoWriter oVideoWriter;
VideoCapture capture;

struct all_data;
class Item;

vector<Mat> MemData; //vector to store data in-memory

inline bool source_op(Item &item);
inline void segment_op(Item &item);
inline void canny1_op(Item &item);
inline void houghT_op(Item &item);
inline void houghP_op(Item &item);
inline void bitwise_op(Item &item);
inline void canny2_op(Item &item);
inline void overlap_op(Item &item);
inline void sink_op(Item &item);
vector<string> set_operators_name();

void init_bench(int argc, char* argv[]);
inline bool file_exists (const string& name);
inline void usage(string name);
void end_bench();


struct all_data{
	//int index;
	Mat *image_p;
	Mat image;
	Mat imgROI;
	Mat contours;
	Mat hough;
	vector<Vec2f> lines;
	LineFinder ld;
	vector<Vec4i> li;
	Mat houghP;
	Mat houghPinv;

	all_data():image_p(NULL){};

	~all_data(){
		lines.clear();
		li.clear();
	}
};

class Item {
	public:
		std::vector<all_data> item_batch;
		vector<double> latency_op;
		volatile unsigned long timestamp;
		unsigned int batch_size;
		unsigned int index;
		Item():
			latency_op(9, 0.0),
			timestamp(0.0),
			batch_size(0),
			index(0)
		{};

		~Item(){
			latency_op.clear();
		}
};

std::vector<string> set_operators_name(){
	vector<string> operator_name_list;
	operator_name_list.push_back("Source ");
	operator_name_list.push_back("Segment");
	operator_name_list.push_back("Canny_1");
	operator_name_list.push_back("HoughT ");
	operator_name_list.push_back("HoughP ");
	operator_name_list.push_back("Bitwise");
	operator_name_list.push_back("Canny_2");
	operator_name_list.push_back("Overlap");
	operator_name_list.push_back("Sink   ");
	return operator_name_list;
}

inline void usage(string name){
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

	string input;
	int opt;
	while ((opt = getopt(argc,argv,"i:t:b:m:F:klfxuh")) != EOF){
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
			case 'm':
				monitoring_time_interval = atoi(optarg);
				enable_monitoring = true;
				break;
			case 'F':
				items_reading_frequency = atoi(optarg);
				memory_source = true;
				break;
			case 'k':
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
	filepath = argv[0];
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

	Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));
	//initialize the VideoWriter object 
	oVideoWriter.open((out_file_path("outputs") + ".avi").c_str(), CV_FOURCC('P','I','M','1'), 20, frameSize, true);

	//load the input to the memory before the stream region for in-memory execution
	if(memory_source){
		while(1){
			Mat image;
			capture >> image;
			if (image.empty()) break;
			MemData.push_back(image);
		}
	}

	/*	int i = 0;
		while(1){
		Mat *image;
	//Data *new_batch = new Data[sizeof Mat];
	capture >> image;


	MemData.push_back(image);
	if (image.empty()) break;
	i++;
	}*/
	//#endif
}

inline bool source_op(Item &item){

	//if last batch included the last item, ends computation
	if(stream_end == true){
		return false;
	}

	unsigned long latency_op;
	item.timestamp = current_time_usecs();
	if(print_latency || latency_to_file){
		latency_op = current_time_usecs();
	}
	
	while(item.batch_size < batch_size){ //batch loop
		all_data item_data;
		
		if(memory_source){
			if(item_count < MemData.size()){
				item_data.image_p = &MemData[item_count];
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
		item.item_batch.resize(item.batch_size+1);
		item.item_batch[item.batch_size] = item_data;
		item.batch_size++;
		item.index = item_count;
		item_count++;
	}

	item.index = input_items;
	

	//if this batch has size 0, ends computation
	if(item.batch_size == 0){
		return false;
	}
	
	// frequency control mechanism
	item_frequency_control(item.timestamp);

	if(print_latency || latency_to_file){
		item.latency_op[0] = (current_time_usecs() - latency_op) / 1000000.0;
	}

	input_items++;	// sent batches
	return true;
}

inline void segment_op(Item &item){
	volatile unsigned long latency_op;
	if(print_latency || latency_to_file){
		latency_op = current_time_usecs();
	}

	unsigned int num_item = 0;
	while(num_item < item.batch_size){ //batch loop
		all_data item_data;
		item_data = item.item_batch[num_item];

		Mat gray;
		if(memory_source){
			cvtColor(*(item_data.image_p), gray,  CV_RGB2GRAY);
		} else {
			cvtColor(item_data.image, gray,  CV_RGB2GRAY);
		}
		vector<string> codes;
		Mat corners;
		findDataMatrix(gray, codes, corners);
		if(memory_source){
			drawDataMatrixCodes(*(item_data.image_p), codes, corners);
			Rect roi(0, item_data.image_p->cols/3, item_data.image_p->cols-1, item_data.image_p->rows - item_data.image_p->cols/3);
			Mat aux = *(item_data.image_p);
			item_data.imgROI = aux(roi);
		} else {
			drawDataMatrixCodes(item_data.image, codes, corners);
			Rect roi(0, item_data.image.cols/3, item_data.image.cols-1, item_data.image.rows - item_data.image.cols/3);
			item_data.imgROI = item_data.image(roi);
		}
		item.item_batch[num_item] = item_data;
		num_item++;
	}

	if(print_latency || latency_to_file){
		item.latency_op[1] = (current_time_usecs() - latency_op) / 1000000.0;
	}
}

inline void canny1_op(Item &item){
	volatile unsigned long latency_op;
	if(print_latency || latency_to_file){
		latency_op = current_time_usecs();
	}

	unsigned int num_item = 0;
	while(num_item < item.batch_size){ //batch loop
		all_data item_data;
		item_data = item.item_batch[num_item];

		//Mat contours;
		Canny(item_data.imgROI, item_data.contours,50,250);
		Mat contoursInv;
		threshold(item_data.contours, contoursInv, 128, 255, THRESH_BINARY_INV);

		item.item_batch[num_item] = item_data;
		num_item++;
	}
	if(print_latency || latency_to_file){
		item.latency_op[2] = (current_time_usecs() - latency_op) / 1000000.0;
	}
}

inline void houghT_op(Item &item){
	volatile unsigned long latency_op;
	if(print_latency || latency_to_file){
		latency_op = current_time_usecs();
	}
	//Hough tranform for line detection with feedback
	//Increase by 25 for the next frame if we found some lines.  
	//This is so we don't miss other lines that may crop up in the next frame
	//but at the same time we don't want to start the feed back loop from scratch. 

	unsigned int num_item = 0;
	while(num_item < item.batch_size){ //batch loop
		all_data item_data;
		item_data = item.item_batch[num_item];

		int houghVote = 200;

		//we lost all lines. reset 
		if (houghVote < 1 || item_data.lines.size() > 2) 
		{ 
			houghVote = 200; 
		}
		else
		{ 
			houghVote += 25;
		} 

		while(item_data.lines.size() < 5 && houghVote > 0)
		{
			HoughLines(item_data.contours, item_data.lines,1,PI/180, houghVote);
			houghVote -= 5;  
		}

		Mat result(item_data.imgROI.size(), CV_8U, Scalar(255));
		item_data.imgROI.copyTo(result);

		//draw the limes
		vector<Vec2f>::const_iterator it;
		Mat hough(item_data.imgROI.size(), CV_8U, Scalar(0));
		it = item_data.lines.begin();

		while(it!=item_data.lines.end()) 
		{
			//first element is distance rho
			float rho= (*it)[0];
			//second element is angle theta	   
			float theta= (*it)[1]; 			
			if( (theta > 0.09 && theta < 1.48) || (theta < 3.14 && theta > 1.66) ) 
			{ 
				//filter to remove vertical and horizontal lines
				//point of intersection of the line with first row
				Point pt1(rho/cos(theta),0);
				//point of intersection of the line with last row
				Point pt2((rho-result.rows*sin(theta))/cos(theta), result.rows);
				//draw a white line
				line(result, pt1, pt2, Scalar(255), 8); 
				line(hough, pt1, pt2, Scalar(255), 8);
			}
			++it;
		}
		item_data.hough = hough;

		item.item_batch[num_item] = item_data;
		num_item++;
	}
	if(print_latency || latency_to_file){
		item.latency_op[3] = (current_time_usecs() - latency_op) / 1000000.0;
	}
}

inline void houghP_op(Item &item){
	volatile unsigned long latency_op;
	if(print_latency || latency_to_file){
		latency_op = current_time_usecs();
	}

	unsigned int num_item = 0;
	while(num_item < item.batch_size){ //batch loop
		all_data item_data;
		item_data = item.item_batch[num_item];

		//set probabilistic Hough parameters
		item_data.ld.setLineLengthAndGap(60,10);
		item_data.ld.setMinVote(4);

		//detect lines
		item_data.li = item_data.ld.findLines(item_data.contours);
		Mat houghP(item_data.imgROI.size(), CV_8U,Scalar(0));
		item_data.ld.setShift(0);
		item_data.ld.drawDetectedLines(houghP);

		item_data.houghP = houghP;

		item.item_batch[num_item] = item_data;
		num_item++;
	}
	if(print_latency || latency_to_file){
		item.latency_op[4] = (current_time_usecs() - latency_op) / 1000000.0;
	}
}
inline void bitwise_op(Item &item){
	volatile unsigned long latency_op;
	if(print_latency || latency_to_file){
		latency_op = current_time_usecs();
	}

	unsigned int num_item = 0;
	while(num_item < item.batch_size){ //batch loop
		all_data item_data;
		item_data = item.item_batch[num_item];
			
		//bitwise AND of the two hough images
		bitwise_and(item_data.houghP, item_data.hough, item_data.houghP);
		Mat houghPinv(item_data.imgROI.size(), CV_8U, Scalar(0));
		//threshold and invert to black lines
		threshold(item_data.houghP, houghPinv, 150, 255, THRESH_BINARY_INV);

		item_data.houghPinv = houghPinv;

		item.item_batch[num_item] = item_data;
		num_item++;
	}

	if(print_latency || latency_to_file){
		item.latency_op[5] = (current_time_usecs() - latency_op) / 1000000.0;
	}
}	

inline void canny2_op(Item &item){
	volatile unsigned long latency_op;
	if(print_latency || latency_to_file){
		latency_op = current_time_usecs();
	}

	unsigned int num_item = 0;
	while(num_item < item.batch_size){ //batch loop
		all_data item_data;
		item_data = item.item_batch[num_item];

		Canny(item_data.houghPinv, item_data.contours, 100, 350);
		item_data.li = item_data.ld.findLines(item_data.contours);

		item.item_batch[num_item] = item_data;
		num_item++;
	}
	if(print_latency || latency_to_file){
		item.latency_op[6] = (current_time_usecs() - latency_op) / 1000000.0;
	}
}

inline void overlap_op(Item &item){
	volatile unsigned long latency_op;
	if(print_latency || latency_to_file){
		latency_op = current_time_usecs();
	}

	unsigned int num_item = 0;
	while(num_item < item.batch_size){ //batch loop
		all_data item_data;
		item_data = item.item_batch[num_item];

		//set probabilistic Hough parameters
		item_data.ld.setLineLengthAndGap(5,2);
		item_data.ld.setMinVote(1);
		if(memory_source){
			item_data.ld.setShift(item_data.image_p->cols/3);
			item_data.ld.drawDetectedLines(*(item_data.image_p));
		} else {
			item_data.ld.setShift(item_data.image.cols/3);
			item_data.ld.drawDetectedLines(item_data.image);
		}
		stringstream stream;
		stream << "Line Segments: " << item_data.lines.size();

		if(memory_source) {
			putText(*(item_data.image_p), stream.str(), Point(10, item_data.image_p->rows-10), 2, 0.8, Scalar(0,0,255),0);
		} else {
			putText(item_data.image, stream.str(), Point(10, item_data.image.rows-10), 2, 0.8, Scalar(0,0,255),0);
		}
		item_data.lines.clear();

		item.item_batch[num_item] = item_data;
		num_item++;
	}

	if(print_latency || latency_to_file){
		item.latency_op[7] = (current_time_usecs() - latency_op) / 1000000.0;
	}
}

inline void sink_op (Item &item){
	volatile unsigned long latency_op;
	if(print_latency || latency_to_file){
		latency_op = current_time_usecs();
	}

	if(item.index != output_items){
		std::cout << "Error while writing output file. Unexpected item." << std::endl;
		exit(1);
	}
	
	if(!memory_source){
		unsigned int num_item = 0;
		while(num_item < item.batch_size){ //batch loop
			oVideoWriter.write(item.item_batch[num_item].image);
			num_item++;
		}
	}
	output_items++;

	if(enable_monitoring){
		monitor_metrics(item.timestamp);
	}
	if(print_latency || latency_to_file){
		item.latency_op[8] = (current_time_usecs() - latency_op) / 1000000.0;
		latency_t latency;
		latency.local_latency = item.latency_op;
		latency.local_total = ((current_time_usecs() - item.timestamp) / 1000000.0);
		latency_vector.push_back(latency);
		item.latency_op.clear();
	}

}

void end_bench(){

	if(memory_source){
		//for(Mat image : MemData){

		while(!MemData.empty()){
			oVideoWriter.write(MemData[0]);
			MemData.erase(MemData.begin());
		}

		if(!MemData.empty())
			MemData.erase(MemData.begin(), MemData.end());
	}
}
