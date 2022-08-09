/**
 * ************************************************************************  
 *  File  : person_recognition_utils_ns.hpp
 *
 *  Title : SPBench version of the nsources Person Recognition
 *
 *  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 *
 *  Date  : July 06, 2021
 *
 * ************************************************************************
**/


#ifndef PERSON_U_NS
#define PERSON_U_NS

/* Includes */
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <sys/time.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

#include <spbench.hpp>

namespace spb{

//inline bool file_exists (const std::string& name);

#define	DEFS_H

/** Flags: **/
#define SHOW_OUTPUT
#define WRITE_OUTPUT
#define WRITE_CSV

/** Inputs: **/
//#define CASCADE_PATH  "inputs/person_recognition/cascades/haarcascade_frontalface_default.xml"
//#define TRAINING_LIST "inputs/person_recognition/obama_faces/list"

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
class Item;
class Source;
struct workload_data;
struct IO_data_struct;

void init_bench(int argc, char* argv[]);
void end_bench();

struct workload_data {
	std::string input_vid;
	std::string cascade_path;
	std::string training_list;
	cv::Ptr<cv::FaceRecognizer> model;
	cv::Size _faceSize;
	std::string inputId;
};

struct IO_data_struct {
	workload_data input_data;
	std::vector<cv::Mat> MemData; //vector to store data in-memory
	cv::VideoWriter oVideoWriter;
};

extern std::vector<IO_data_struct> IO_data_vec;

struct item_data {
	cv::Mat *image_p;
	cv::Mat image;
	std::vector<cv::Rect> faces;
	unsigned int index;
	unsigned int sourceId;

	~item_data(){
		faces.clear();
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
		unsigned int sourceQueueSize;

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
			setSourceName(IO_data_vec[sourceId].input_data.inputId);
			source_metrics = init_metrics();
			source_metrics.sourceId = sourceId;
			source_metrics.source_name = getSourceName();
			metrics_vec.push_back(source_metrics);
			source_thread = std::thread(&Source::source_op, this);
			isRunning = true;
			printStatus();
		}

		~Source(){}
		
		void init(){
			if(isRunning){
				std::cout << "Failed to use init(), this source is already running!" << std::endl;
				return;
			}

			checkInput();
			setSourceName(IO_data_vec[sourceId].input_data.inputId);
			source_metrics = init_metrics();
			source_metrics.sourceId = sourceId;
			source_metrics.source_name = getSourceName();
			metrics_vec.push_back(source_metrics);

			source_thread = std::thread(&Source::source_op, this);
			isRunning = true;

			printStatus();
		}

		Item getItem(){
			
			Item item;
			
			if(depleted() || !isRunning) return item;

			item = sourceQueue.dequeue();
			item.sourceId = sourceId;

			if (sourceQueue.front().isLastItem()){
				std::cout << "\n -> Source depleted: " << getSourceName() << std::endl;
				sourceDepleted = true;
				tryToJoin();
			}
			return item;
		}

		void setQueueMaxSize(unsigned int newQueueSize){
			
			if(newQueueSize < 0){
				std::cout << " Queue size must be 0 (unlimited) or higher.\n";
				return;
			}

			if(newQueueSize < sourceQueue.size()){
				std::cout << " You cannot set a max. queue size lower than the number of current items in the queue.\n";
				std::cout << " Number of items in the queue: " << sourceQueue.size() << std::endl;
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


} //end of namespace spb

#endif