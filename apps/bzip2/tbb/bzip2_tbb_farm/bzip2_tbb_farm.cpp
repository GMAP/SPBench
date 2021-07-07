#include <metrics.hpp>
#include <bzip2.hpp>
#include <tbb/pipeline.h>
#include "tbb/task_scheduler_init.h"
using namespace tbb;

class stage1_comp : public tbb::filter{
public:
	stage1_comp() : tbb::filter(tbb::filter::serial_in_order) {}
	void* operator() (void*){
		while(bytesLeft > 0){
			Item * item = new Item();
			if(!read_comp_op(*item)) break;
			return item;
		}
		return NULL;
	}
};

class stage2_comp : public tbb::filter{
public:
	stage2_comp() : tbb::filter(tbb::filter::parallel) {}
	void* operator() (void* new_item){
		Item * item = static_cast <Item*> (new_item);
		compress_op(*item);
		return item;
	}
};

class stage3_comp : public tbb::filter{
public:
	stage3_comp() : tbb::filter(tbb::filter::serial_in_order) {}
	void* operator() (void* new_item){
		Item * item = static_cast <Item*> (new_item);
		write_comp_op(*item);
		return NULL;
	}
};

void compress(){

	data_metrics metrics = init_metrics();

	/*----------TBB region----------*/

	task_scheduler_init init_parallel(nthreads);

	pipeline pipeline;

	stage1_comp read;
	pipeline.add_filter(read);
	stage2_comp compress;
	pipeline.add_filter(compress);
	stage3_comp write;
	pipeline.add_filter(write);

	pipeline.run(nthreads*10);

	/*------------------------------*/
	stop_metrics(metrics);
}

class stage1_decomp : public tbb::filter{
public:
	stage1_decomp() : tbb::filter(tbb::filter::serial_in_order) {}
	void* operator() (void*){
		while(item_count < bz2NumBlocks){
			Item * item = new Item();
			if(!read_decomp_op(*item)) break;
			return item;
		}
		return NULL;
	}
};

class stage2_decomp : public tbb::filter{
public:
	stage2_decomp() : tbb::filter(tbb::filter::parallel) {}
	void* operator() (void* new_item){
		Item * item = static_cast <Item*> (new_item);
		decompress_op(*item);
		return item;
	}
};

class stage3_decomp : public tbb::filter{
public:
	stage3_decomp() : tbb::filter(tbb::filter::serial_in_order) {}
	void* operator() (void* new_item){
		Item * item = static_cast <Item*> (new_item);
		write_decomp_op(*item);
		return NULL;
	}
};

void decompress(){

	data_metrics metrics = init_metrics();

	/*----------TBB region----------*/

	task_scheduler_init init_parallel(nthreads);

	pipeline pipeline;

	stage1_decomp read;
	pipeline.add_filter(read);
	stage2_decomp decompress;
	pipeline.add_filter(decompress);
	stage3_decomp write;
	pipeline.add_filter(write);

	pipeline.run(nthreads*10);

	/*------------------------------*/
	stop_metrics(metrics);
}

int main (int argc, char* argv[]){
	bzip2_main(argc, argv);
	return 0;
}