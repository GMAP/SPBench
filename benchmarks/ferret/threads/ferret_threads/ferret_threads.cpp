#include <ferret.hpp>
#include "SPar_Shared_Queue.hpp"

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

	// Stage 1
	std::thread stage1(emitter,queue1);
	// Stage 2
	std::vector<std::thread> stage2;
	for(int i=0;i < spb::nthreads; i++)
		stage2.push_back(std::thread(worker,queue1,queue2));
	// Stage 3
	std::thread stage3(collector,queue2);

	stage1.join();
 	for (auto& t : stage2)
    	t.join();
	stage3.join();

    spb::Metrics::stop();
	spb::end_bench();
    return 0;
}