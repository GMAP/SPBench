/**
 * ************************************************************************  
 *  File  : person_recognition_utils.hpp
 *
 *  Title : SPBench version of the Person Recognition
 *
 *  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 *
 *  Date  : July 06, 2021
 *
 * ************************************************************************
**/

#ifndef PERSON_U
#define PERSON_U 

/** Includes: **/
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

#include <spbench.hpp>

namespace spb{

#define	DEFS_H

/** Flags: **/
#define SHOW_OUTPUT
#define WRITE_OUTPUT
#define WRITE_CSV

/** Inputs: **/
//#define input_data.cascade_path  "inputs/person_recognition/cascades/haarcascade_frontalface_default.xml"
//#define input_data.training_list "inputs/person_recognition/obama_faces/list"

/** Input video: **/
#define START_FRAME  2400
#define END_FRAME    4800
#define FRAMES_DELTA -1

/** Output video: **/
#define OUT_FPS    15
#define OUT_FOURCC CV_FOURCC('M','J','P','G') //codec

/** Colors, fonts, lines... **/
#define NO_MATCH_COLOR    Scalar(0,0,255) //red
#define MATCH_COLOR       Scalar(0,255,0) //green
#define FACE_RADIUS_RATIO 0.75
#define CIRCLE_THICKNESS  2.5
#define LINE_TYPE         CV_AA
#define FONT              FONT_HERSHEY_PLAIN
#define FONT_COLOR        Scalar(255,255,255)
#define THICKNESS_TITLE   1.9
#define SCALE_TITLE       1.9
#define POS_TITLE         cvPoint(10, 30)
#define THICKNESS_LINK    1.6
#define SCALE_LINK        1.3
#define POS_LINK          cvPoint(10, 55)

/** Face detector: **/
#define DET_SCALE_FACTOR   1.01
#define DET_MIN_NEIGHBORS  40
#define DET_MIN_SIZE_RATIO 0.06
#define DET_MAX_SIZE_RATIO 0.18

/** LBPH face recognizer: **/
#define LBPH_RADIUS    3
#define LBPH_NEIGHBORS 8
#define LBPH_GRID_X    8
#define LBPH_GRID_Y    8
#define LBPH_THRESHOLD 180.0

#define NUMBER_OF_OPERATORS 4

struct item_data;
struct workload_data;
class Item;
class Source;
class Sink;

extern cv::Ptr<cv::FaceRecognizer> model;
extern cv::Size _faceSize;

void init_bench(int argc, char* argv[]);
void end_bench();

struct workload_data {
	std::string input_vid;
	std::string cascade_path;
	std::string training_list;
	std::string id;
};

extern workload_data input_data;

struct item_data {
	cv::Mat *image_p;
	cv::Mat image;
	std::vector<cv::Rect> faces;
	unsigned int index;

	item_data():
		image_p(NULL),
		index(0)
	{};

	~item_data(){
		faces.clear();
	}
};

/* This class implements an Item */
class Item : public Batch{
public:
	std::vector<item_data> item_batch;

	Item():Batch(NUMBER_OF_OPERATORS){};

	~Item(){}
};

class Source{
public:
	static long source_item_timestamp;
	static int operator_id;
	static bool op(Item &item);
	Source(){}
	virtual ~Source(){}
};

class Sink{
public:
	static void op(Item &item);
	static int operator_id;
	Sink(){}
	virtual ~Sink(){}
};

} //end of namespace spb

#endif