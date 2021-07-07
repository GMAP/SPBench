#include <metrics.hpp>
#include <lane_detection.hpp>
#include "tbb/pipeline.h"
#include "tbb/task_scheduler_init.h"
using namespace tbb;

class stage1 : public tbb::filter{
public:
	stage1() : tbb::filter(tbb::filter::serial_in_order) {}
	void* operator() (void*){
		while(1){
			Item * item = new Item();
			if (!source_op(*item)) break;
			return item;
		}
		return NULL;
	}
};

class stage2 : public tbb::filter{
public:
        stage2() : tbb::filter(tbb::filter::parallel) {}
        void* operator() (void* new_item){
                Item * item = static_cast <Item*> (new_item);
         	segment_op(*item);
                canny1_op(*item);
	 	houghT_op(*item);
                houghP_op(*item);
                bitwise_op(*item);
                canny2_op(*item);
                overlap_op(*item);
                return item;
        }
};


class stage3 : public tbb::filter{
public:
	stage3() : tbb::filter(tbb::filter::serial_in_order) {}
	void* operator() (void* new_item){
		Item * item = static_cast <Item*> (new_item);
		sink_op(*item);
		return NULL;
	}
};

int main (int argc, char* argv[]){

	// Disabling internal OpenCV's support for multithreading.
	setNumThreads(0);

	init_bench(argc, argv); //Initializations

	data_metrics metrics = init_metrics(); //UPL and throughput

	//TBB code:
	task_scheduler_init init_parallel(nthreads);

	pipeline pipeline;

	stage1 read;
	pipeline.add_filter(read);
	stage2 process;
	pipeline.add_filter(process);
	stage3 write;
	pipeline.add_filter(write);

	pipeline.run(nthreads*10);

	stop_metrics(metrics);

	end_bench();

	return 0;
}
