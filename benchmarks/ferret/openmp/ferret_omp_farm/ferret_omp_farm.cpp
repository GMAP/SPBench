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
    #define QUEUESIZE 512
#else
    #define QUEUESIZE 1
#endif

struct data{
	spb::Item item;
	bool omp_spar_eos;
};

void emitter(SParSharedQueue<struct data> * queue1){
	struct data * local;
	while(1){
		spb::Item item;
		if(!spb::Source::op(item)) {
			queue1->NotifyEOS();
			break;
		}
		local = new struct data();
		local->omp_spar_eos = false;
		local->item = item;
		queue1->Add(local);
	}
}

void worker(SParSharedQueue<struct data> * queue1, SParSharedQueue<struct data> * queue2){
	struct data * local;
	while(1){
		local = queue1->Remove();
		if(local->omp_spar_eos){
			queue2->NotifyEOS();
			break;
		}
        spb::Segmentation::op(local->item);
        spb::Extract::op(local->item);
        spb::Vectorization::op(local->item);
        spb::Rank::op(local->item);
		queue2->Add(local);
	}
}

void collector(SParSharedQueue<struct data> * queue2){
	struct data * local;
	while(1){
		local = queue2->Remove();
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

	omp_set_num_threads(spb::nthreads+2);
	#pragma omp parallel shared(queue1,queue2)
	#pragma omp single nowait
	#pragma omp taskgroup
	{
		// Stage 1
		for(int _i=0;_i<1;_i++){
			#pragma omp task
			{
				emitter(queue1);
			}
		}
		// Stage 2
		for(int i=0;i < spb::nthreads; i++){
			#pragma omp task 
			{
				worker(queue1,queue2);
			}
		}
		// Stage 3
		for(int _i=0;_i<1;_i++){
			#pragma omp task
			{
				collector(queue2);
			}
		}
	}

    spb::Metrics::stop();
	spb::end_bench();
    return 0;
}