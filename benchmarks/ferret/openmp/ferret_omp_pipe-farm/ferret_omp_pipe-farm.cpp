/*
Author: Renato Hoffmann
Email: <renato.hoffmann@edu.pucrs.br>
December 2022

Although validated, there are no guarantees that this code will work. 
In case of questions, please contact via email.
*/

#include <ferret.hpp>
#include "SPar_Shared_Queue.hpp"
#include <omp.h>

#ifdef ONDEMAND
    #define QUEUESIZE 1
#else
    #define QUEUESIZE 512
#endif

struct data{
	spb::Item item;
	bool omp_spar_eos;
};

void emitter(SParSharedQueue<struct data> * outQueue){
	struct data * local;
	while(1){
		spb::Item item;
		if(!spb::Source::op(item)) {
			outQueue->NotifyEOS();
			break;
		}
		local = new struct data();
		local->omp_spar_eos = false;
		local->item = item;
		outQueue->Add(local);
	}
}

void segmentation(SParSharedQueue<struct data> * inQueue, SParSharedQueue<struct data> * outQueue){
	struct data * local;
	while(1){
		local = inQueue->Remove();
		if(local->omp_spar_eos){
			outQueue->NotifyEOS();
			break;
		}
        spb::Segmentation::op(local->item);
		outQueue->Add(local);
	}
}

void extract(SParSharedQueue<struct data> * inQueue, SParSharedQueue<struct data> * outQueue){
	struct data * local;
	while(1){
		local = inQueue->Remove();
		if(local->omp_spar_eos){
			outQueue->NotifyEOS();
			break;
		}
        spb::Extract::op(local->item);
		outQueue->Add(local);
	}
}

void vectorization(SParSharedQueue<struct data> * inQueue, SParSharedQueue<struct data> * outQueue){
	struct data * local;
	while(1){
		local = inQueue->Remove();
		if(local->omp_spar_eos){
			outQueue->NotifyEOS();
			break;
		}
        spb::Vectorization::op(local->item);
		outQueue->Add(local);
	}
}

void rank(SParSharedQueue<struct data> * inQueue, SParSharedQueue<struct data> * outQueue){
	struct data * local;
	while(1){
		local = inQueue->Remove();
		if(local->omp_spar_eos){
			outQueue->NotifyEOS();
			break;
		}
        spb::Rank::op(local->item);
		outQueue->Add(local);
	}
}

void collector(SParSharedQueue<struct data> * inQueue){
	struct data * local;
	while(1){
		local = inQueue->Remove();
		if(local->omp_spar_eos){
			break;
		}
        spb::Sink::op(local->item);
        delete local;
	}
}

int main(int argc, char *argv[]) {

    spb::init_bench(argc, argv);
	spb::Metrics::init();

	SParSharedQueue<struct data> * queue1 = new SParSharedQueue<struct data>(QUEUESIZE*spb::nthreads,1);
	SParSharedQueue<struct data> * queue2 = new SParSharedQueue<struct data>(QUEUESIZE*spb::nthreads,spb::nthreads);
	SParSharedQueue<struct data> * queue3 = new SParSharedQueue<struct data>(QUEUESIZE*spb::nthreads,spb::nthreads);
	SParSharedQueue<struct data> * queue4 = new SParSharedQueue<struct data>(QUEUESIZE*spb::nthreads,spb::nthreads);
	SParSharedQueue<struct data> * queue5 = new SParSharedQueue<struct data>(QUEUESIZE*spb::nthreads,spb::nthreads);

	omp_set_num_threads((spb::nthreads * 4) + 2);
	#pragma omp parallel shared(queue1,queue2)
	#pragma omp single nowait
	#pragma omp taskgroup
	{
		// First stage
		for(int _i=0;_i<1;_i++){
			#pragma omp task
			{
				emitter(queue1);
			}
		}
		// Middle stages
		for(int i=0;i < spb::nthreads; i++){
			#pragma omp task 
			{
				segmentation(queue1,queue2);
			}
			#pragma omp task 
			{
				extract(queue2,queue3);
			}
			#pragma omp task 
			{
				vectorization(queue3,queue4);
			}
			#pragma omp task 
			{
				rank(queue4,queue5);
			}
		}
		// Last stage 3
		for(int _i=0;_i<1;_i++){
			#pragma omp task
			{
				collector(queue5);
			}
		}
	}

    spb::Metrics::stop();
	spb::end_bench();
    return 0;
}