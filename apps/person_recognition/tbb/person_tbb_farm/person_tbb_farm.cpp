#include <person_recognition.hpp>
#include <tbb/pipeline.h>
#include "tbb/task_scheduler_init.h"

class stage1 : public tbb::filter{
public:
	stage1() : tbb::filter(tbb::filter::serial_in_order) {}
	void* operator() (void*){
		while(1){
			spb::Item * item = new spb::Item();
			if (!spb::Source::op(*item)) break;
			return item;
		}
		return NULL;
	}
};

class stage2 : public tbb::filter{
public:
	stage2() : tbb::filter(tbb::filter::parallel) {}
	void* operator() (void* new_item){
		spb::Item * item = static_cast <spb::Item*> (new_item);
		//detect faces in the image:
		spb::Detect::op(*item);
		
		//analyze each detected face:
		spb::Recognize::op(*item);
		return item;
	}
};

class stage3 : public tbb::filter{
public:
	stage3() : tbb::filter(tbb::filter::serial_in_order) {}
	void* operator() (void* new_item){
		spb::Item * item = static_cast <spb::Item*> (new_item);
		spb::Sink::op(*item);
		return NULL;
	}
};

int main (int argc, char* argv[]){
	//Disabling internal OpenCV's support for multithreading. 
	cv::setNumThreads(0);

	spb::init_bench(argc, argv);
	
	spb::Metrics::init();

	//TBB code:
	tbb::task_scheduler_init init_parallel(spb::nthreads);

	tbb::pipeline pipeline;

	stage1 read;
	pipeline.add_filter(read);
	stage2 process;
	pipeline.add_filter(process);
	stage3 write;
	pipeline.add_filter(write);

	pipeline.run(spb::nthreads*10);

	spb::Metrics::stop();
	spb::end_bench();
	return 0;
}

