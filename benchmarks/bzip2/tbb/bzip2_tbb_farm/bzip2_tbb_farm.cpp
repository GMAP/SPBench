#include <bzip2.hpp>
#include <tbb/pipeline.h>
#include "tbb/task_scheduler_init.h"

class stage1_comp : public tbb::filter{
public:
	stage1_comp() : tbb::filter(tbb::filter::serial_in_order) {}
	void* operator() (void*){
		while(1){
			spb::Item * item = new spb::Item();
			if(!spb::Source::op(*item)) break;
			return item;
		}
		return NULL;
	}
};

class stage2_comp : public tbb::filter{
public:
	stage2_comp() : tbb::filter(tbb::filter::parallel) {}
	void* operator() (void* new_item){
		spb::Item * item = static_cast <spb::Item*> (new_item);
		spb::Compress::op(*item);
		return item;
	}
};

class stage3_comp : public tbb::filter{
public:
	stage3_comp() : tbb::filter(tbb::filter::serial_in_order) {}
	void* operator() (void* new_item){
		spb::Item * item = static_cast <spb::Item*> (new_item);
		spb::Sink::op(*item);
		return NULL;
	}
};

void compress(){

	spb::Metrics::init();

	/*----------TBB region----------*/

	tbb::task_scheduler_init init_parallel(spb::nthreads);

	tbb::pipeline pipeline;

	stage1_comp read;
	pipeline.add_filter(read);
	stage2_comp compress;
	pipeline.add_filter(compress);
	stage3_comp write;
	pipeline.add_filter(write);

	pipeline.run(spb::nthreads*10);

	/*------------------------------*/
	spb::Metrics::stop();
}

class stage1_decomp : public tbb::filter{
public:
	stage1_decomp() : tbb::filter(tbb::filter::serial_in_order) {}
	void* operator() (void*){
		while(1){
			spb::Item * item = new spb::Item();
			if(!spb::Source_d::op(*item)) break;
			return item;
		}
		return NULL;
	}
};

class stage2_decomp : public tbb::filter{
public:
	stage2_decomp() : tbb::filter(tbb::filter::parallel) {}
	void* operator() (void* new_item){
		spb::Item * item = static_cast <spb::Item*> (new_item);
		spb::Decompress::op(*item);
		return item;
	}
};

class stage3_decomp : public tbb::filter{
public:
	stage3_decomp() : tbb::filter(tbb::filter::serial_in_order) {}
	void* operator() (void* new_item){
		spb::Item * item = static_cast <spb::Item*> (new_item);
		spb::Sink_d::op(*item);
		return NULL;
	}
};

void decompress(){

	spb::Metrics::init();

	/*----------TBB region----------*/

	tbb::task_scheduler_init init_parallel(spb::nthreads);

	tbb::pipeline pipeline;

	stage1_decomp read;
	pipeline.add_filter(read);
	stage2_decomp decompress;
	pipeline.add_filter(decompress);
	stage3_decomp write;
	pipeline.add_filter(write);

	pipeline.run(spb::nthreads*10);

	/*------------------------------*/
	spb::Metrics::stop();
}

int main (int argc, char* argv[]){
	spb::bzip2_main(argc, argv);
	return 0;
}