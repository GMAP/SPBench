#include <bzip2.hpp>
#include <queue>
#include "SPar_Shared_Queue.hpp"
#include <omp.h>

#ifdef ONDEMAND
    #define QUEUESIZE 512
#else
    #define QUEUESIZE 1
#endif

struct data{
	spb::Item item;
	bool omp_spar_eos;
	int order_id;
};

struct compare_task_data{
	bool operator()(struct data * t1, struct data * t2){
		return (t1->order_id > t2->order_id);
	}
	bool operator()(const struct data & t1, const struct data & t2){
		return (t1.order_id > t2.order_id);
	}
	bool operator()(struct data && t1, struct data && t2){
		return (t1.order_id > t2.order_id);
	}
};

void comp_emitter(SParSharedQueue<struct data> * queue1){
	struct data * local;
	int curr_id = 0;
	while(1){
		spb::Item item;
		if(!spb::Source::op(item)) {
			queue1->NotifyEOS();
			break;
		}
		local = new struct data();
		local->omp_spar_eos = false;
		local->item = item;
		local->order_id = curr_id;
		queue1->Add(local);
		curr_id++;
	}
}

void comp_worker(SParSharedQueue<struct data> * queue1, SParSharedQueue<struct data> * queue2){
	struct data * local;
	while(1){
		local = queue1->Remove();
		if(local->omp_spar_eos){
			queue2->NotifyEOS();
			break;
		}
		spb::Compress::op(local->item);
		queue2->Add(local);
	}
}

void comp_collector(SParSharedQueue<struct data> * queue2){
	struct data * local;
	std::priority_queue<struct data*,std::deque<struct data*>,compare_task_data> pqueue_buffer;
	int local_id = 0;
	while(1){
		local = queue2->Remove();
		if(local->omp_spar_eos){
			break;
		}
		
		while(1){
			if(local->order_id!=local_id){
				pqueue_buffer.push(local);
				break;
			}
			spb::Sink::op(local->item);
			local_id++;
			delete local;
			if(pqueue_buffer.empty() 
				|| (local_id < pqueue_buffer.top()->order_id) )
				break; 

			local = pqueue_buffer.top();
			pqueue_buffer.pop();

		}

	}
}

void compress(){

	spb::Metrics::init();

	SParSharedQueue<struct data> * queue1 = new SParSharedQueue<struct data>(QUEUESIZE*spb::nthreads,1);
	SParSharedQueue<struct data> * queue2 = new SParSharedQueue<struct data>(QUEUESIZE*spb::nthreads,spb::nthreads);


	omp_set_num_threads(spb::nthreads+2);
	#pragma omp parallel shared(queue1,queue2)
	#pragma omp single nowait
	#pragma omp taskgroup
	{
		// Stage 1
		for(int _i=0;_i<1;_i++){
			#pragma omp task
			{
				comp_emitter(queue1);
			}
		}
		// Stage 2
		for(int i=0;i < spb::nthreads; i++){
			#pragma omp task 
			{
				comp_worker(queue1,queue2);
			}
		}
		// Stage 3
		for(int _i=0;_i<1;_i++){
			#pragma omp task
			{
				comp_collector(queue2);
			}
		}
	}

	spb::Metrics::stop();
}

void decomp_emitter(SParSharedQueue<struct data> * queue1){
	struct data * local;
	int curr_id = 0;
	while(1){
		spb::Item item;
		if(!spb::Source_d::op(item)) {
			queue1->NotifyEOS();
			break;
		}
		local = new struct data();
		local->omp_spar_eos = false;
		local->item = item;
		local->order_id = curr_id;
		queue1->Add(local);
		curr_id++;
	}
}

void decomp_worker(SParSharedQueue<struct data> * queue1, SParSharedQueue<struct data> * queue2){
	struct data * local;
	while(1){
		local = queue1->Remove();
		if(local->omp_spar_eos){
			queue2->NotifyEOS();
			break;
		}
		spb::Decompress::op(local->item);
		queue2->Add(local);
	}
}

void decomp_collector(SParSharedQueue<struct data> * queue2){
	struct data * local;
	int local_id = 0;
	std::priority_queue<struct data*,std::deque<struct data*>,compare_task_data> pqueue_buffer;
	while(1){
		local = queue2->Remove();
		if(local->omp_spar_eos){
			break;
		}
		
		while(1){
			if(local->order_id!=local_id){
				pqueue_buffer.push(local);
				break;
			}
			spb::Sink_d::op(local->item);
			local_id++;
			delete local;
			if(pqueue_buffer.empty() 
				|| (local_id < pqueue_buffer.top()->order_id) )
				break; 

			local = pqueue_buffer.top();
			pqueue_buffer.pop();

		}

	}
}

void decompress(){
	spb::Metrics::init();

	SParSharedQueue<struct data> * queue1 = new SParSharedQueue<struct data>(QUEUESIZE*spb::nthreads,1);
	SParSharedQueue<struct data> * queue2 = new SParSharedQueue<struct data>(QUEUESIZE*spb::nthreads,spb::nthreads);

	omp_set_num_threads(spb::nthreads+2);
	#pragma omp parallel shared(queue1,queue2)
	#pragma omp single nowait
	#pragma omp taskgroup
	{
		// Stage 1
		for(int _i=0;_i<1;_i++){
			#pragma omp task
			{
				decomp_emitter(queue1);
			}
		}
		// Stage 2
		for(int i=0;i < spb::nthreads; i++){
			#pragma omp task 
			{
				decomp_worker(queue1,queue2);
			}
		}
		// Stage 3
		for(int _i=0;_i<1;_i++){
			#pragma omp task
			{
				decomp_collector(queue2);
			}
		}
	}

	spb::Metrics::stop();
}

int main (int argc, char* argv[]){

	spb::bzip2_main(argc, argv);
	return 0;
}