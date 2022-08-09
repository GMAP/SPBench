/**
 * ************************************************************************  
 *  File  : lane_detection_utils.hpp
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
#ifndef LANE_U
#define LANE_U 

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

#include <spbench.hpp>

#define FRAME_ID_INITIALIZATION -1

#define NUMBER_OF_OPERATORS 9

namespace spb {

struct item_data;
struct IO_data_struct;

class Item;
class Source;
class Sink;

void init_bench(int argc, char* argv[]);
void end_bench();
void set_operators_name();
void usage(std::string);

extern std::vector<IO_data_struct> IO_data_vec;

struct IO_data_struct{
	std::string inputFile;
	std::vector<cv::Mat> MemData; //vector to store data in-memory
	cv::VideoWriter oVideoWriter;
};

struct item_data{
	cv::Mat *image_p;
	cv::Mat image;
	cv::Mat imgROI;
	cv::Mat contours;
	cv::Mat hough;
	std::vector<cv::Vec2f> lines;
	LineFinder ld;
	std::vector<cv::Vec4i> li;
	cv::Mat houghP;
	cv::Mat houghPinv;
	unsigned int index;

	item_data():
		image_p(NULL),
		index(0)
	{};

	~item_data(){
		lines.clear();
		li.clear();
	}
};

/* This class implements an Item */
class Item : public Batch, public NsItem{
public:
	std::vector<item_data> item_batch;

	Item():Batch(NUMBER_OF_OPERATORS){};

	~Item(){}
};

class Source : public SuperSource{
	private:
		int sourceQueueSize;

		void source_op();
		void printStatus();

		void checkInput(){
			if(IO_data_vec.size() < sourceObjCounter){
				std::cout << " Error!! You must provide an input for each source." << std::endl;
				std::cout << " - Created sources: " << sourceObjCounter << std::endl;
				std::cout << " - Given inputs: " << IO_data_vec.size() << std::endl;
				std::cout << std::endl;
				exit(0);
			}
			return;
		}

		void tryToJoin(){ // If thread Object is Joinable then Join that thread.
			if (source_thread.joinable()){
				source_thread.join();
			}
		}

	public:
		
		std::vector<Metrics::item_metrics_data> sourceLatencyVector;
		concurrent::queue::blocking_queue<Item> sourceQueue;

		Source() : SuperSource(){
			setBatchSize(SPBench::getBatchSize());
			setBatchInterval(SPBench::getBatchInterval());
			setQueueMaxSize(0);
			setFrequency(SPBench::getFrequency());
			setSourceId(sourceObjCounter);
			setSourceDepleted(false);
			setRunningState(false);
			sourceObjCounter++;
		}

		Source(int batchSize, float batchInterval, int queueSize, long frequency) : SuperSource(){
			sourceDepleted = false;
			sourceId = sourceObjCounter;
			sourceObjCounter++;
			checkInput();
			setFrequency(frequency);
			setQueueMaxSize(queueSize);
			setBatchSize(batchSize);
			setBatchInterval(batchInterval);
			setSourceName(remove_extension(base_name(IO_data_vec[sourceId].inputFile)));
			source_metrics = init_metrics();
			source_metrics.sourceId = sourceId;
			source_metrics.source_name = getSourceName();
			metrics_vec.push_back(source_metrics);
			source_thread = std::thread(&Source::source_op, this);
			setRunningState(true);
			printStatus();
		}

		~Source(){}
		
		void init(){
			if(getRunningState()){
				std::cout << "Failed to use init(), this source is already running!" << std::endl;
				return;
			}
			checkInput();
			setSourceName(remove_extension(base_name(IO_data_vec[sourceId].inputFile)));
			source_metrics = init_metrics();
			source_metrics.sourceId = sourceId;
			source_metrics.source_name = getSourceName();
			metrics_vec.push_back(source_metrics);

			source_thread = std::thread(&Source::source_op, this);
			setRunningState(true);

			printStatus();
		}

		Item getItem(){
			
			Item item;
			
			if(depleted() || !getRunningState()) return item;

			item = sourceQueue.dequeue();
			item.sourceId = sourceId;

			if (sourceQueue.front().isLastItem()){
				std::cout << "\n -> Source depleted: " << getSourceName() << std::endl;
				sourceDepleted = true;
				tryToJoin();
			}
			return item;
		}

		void setQueueMaxSize(int newQueueSize){
			
			if(newQueueSize < 0){
				std::cerr << " Queue size must be 0 (unlimited) or higher.\n";
				return;
			}

			if(newQueueSize < sourceQueue.size()){
				std::cerr << " You cannot set a max. queue size lower than the number of current items in the queue.\n";
				std::cerr << " Number of items in the queue: " << sourceQueue.size() << std::endl;
				return;
			}

			sourceQueueSize = newQueueSize;
			sourceQueue.setCapacity(sourceQueueSize);
		}
};

class Sink{
public:
	static void op(Item &item);
	Sink(){}
	virtual ~Sink(){}
};

} // end of namespace spb

#endif