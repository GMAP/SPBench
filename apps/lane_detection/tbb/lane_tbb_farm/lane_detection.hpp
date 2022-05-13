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
#ifndef LANE_H
#define LANE_H

#include <lane_detection_utils.hpp>

namespace spb{

class Segment;
class Canny1;
class HoughT;
class HoughP;
class Bitwise;
class Canny2;
class Overlap;

class Segment{
private:
	static inline void segment_op(spb::item_data &item);
public:
	static void op(spb::Item &item);
	Segment(spb::Item &item){
		op(item);
	}
	Segment(){};
	virtual ~Segment(){}
};

class Canny1{
private:
	static inline void canny1_op(spb::item_data &item);
public:
	static void op(spb::Item &item);
	Canny1(spb::Item &item){
		op(item);
	}
	Canny1(){};
	virtual ~Canny1(){}
};

class HoughT{
private:
	static inline void houghT_op(spb::item_data &item);
public:
	static void op(spb::Item &item);
	HoughT(spb::Item &item){
		op(item);
	}
	HoughT(){};
	virtual ~HoughT(){}
};

class HoughP{
private:
	static inline void houghP_op(spb::item_data &item);
public:
	static void op(spb::Item &item);
	HoughP(spb::Item &item){
		op(item);
	}
	HoughP(){};
	virtual ~HoughP(){}
};

class Bitwise{
private:
	static inline void bitwise_op(spb::item_data &item);
public:
	static void op(spb::Item &item);
	Bitwise(spb::Item &item){
		op(item);
	}
	Bitwise(){};
	virtual ~Bitwise(){}
};

class Canny2{
private:
	static inline void canny2_op(spb::item_data &item);
public:
	static void op(spb::Item &item);
	Canny2(spb::Item &item){
		op(item);
	}
	Canny2(){};
	virtual ~Canny2(){}
};

class Overlap{
private:
	static inline void overlap_op(spb::item_data &item);
public:
	static void op(spb::Item &item);
	Overlap(spb::Item &item){
		op(item);
	}
	Overlap(){};
	virtual ~Overlap(){}
};

} // end of namespace spb
#endif