#include <metrics.hpp>
#include <lane_detection.hpp>

#include <ff/ff.hpp>

using namespace ff;

struct Emitter: ff_node_t<Item>{

	Item * svc(Item * task){
		while (1){
			Item * item = new Item();
			if (!source_op(*item)) break;
		    ff_send_out(item);
		}
		return EOS;
	}
};

struct Worker: ff_node_t<Item>{
	Item * svc(Item * item){
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

struct Collector: ff_node_t<Item>{
	Item * svc(Item * item){
		sink_op(*item);
		return GO_ON;
	}
}Collector;

int main (int argc, char* argv[]){

	// Disabling internal OpenCV's support for multithreading.
	setNumThreads(0);

	init_bench(argc, argv); //Initializations

	data_metrics metrics = init_metrics(); //UPL and throughput

	vector<unique_ptr<ff_node>> workers;

	for(int i=0; i<nthreads; i++){
		workers.push_back(make_unique<Worker>());
	}

	ff_OFarm<Item> farm(move(workers));

	Emitter E;
	farm.add_emitter(E);
	farm.add_collector(Collector);

	farm.set_scheduling_ondemand();

	if(farm.run_and_wait_end()<0){
		cout << "error running pipe";
	}

	stop_metrics(metrics);

	end_bench();

	return 0;
}