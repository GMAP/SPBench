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
#include "linefinder.h"
#include <spbench.hpp>

#define FRAME_ID_INITIALIZATION -1

#define NUMBER_OF_OPERATORS 9

bool file_exists (const std::string&);

namespace spb {

struct item_data;

class Item;
class Source;
class Sink;

void init_bench(int argc, char* argv[]);
void end_bench();
void set_operators_name();
void usage(std::string);

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

class Item : public Batch{
public:
	std::vector<item_data> item_batch;

	Item():Batch(NUMBER_OF_OPERATORS){};

	~Item(){}
};


class Source{
public:
	static long source_item_timestamp;
	static bool op(Item &item);
	Source(){}
	virtual ~Source(){}
};

class Sink{
public:
	static void op(Item &item);
	Sink(){}
	virtual ~Sink(){}
};

} // end of namespace spb

#endif