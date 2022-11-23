#include <lane_detection.hpp>

#include <ff/ff.hpp>

struct Emitter: ff::ff_node_t<spb::Item>{
	spb::Item * svc(spb::Item * task){
		while (1){
			spb::Item * item = new spb::Item();
			if (!spb::Source::op(*item)) break;
		    ff_send_out(item);
		}
		return EOS;
	}
};

struct Worker: ff::ff_node_t<spb::Item>{
	spb::Item * svc(spb::Item * item){
		spb::Segment::op(*item);
		spb::Canny1::op(*item);
		spb::HoughT::op(*item);
		spb::HoughP::op(*item);
		spb::Bitwise::op(*item);
		spb::Canny2::op(*item);
		spb::Overlap::op(*item);
		return item;
	}
};

struct Collector: ff::ff_node_t<spb::Item>{
	spb::Item * svc(spb::Item * item){
		spb::Sink::op(*item);
		delete item;
		return GO_ON;
	}
}Collector;

int main (int argc, char* argv[]){

	// Disabling internal OpenCV's support for multithreading.
	cv::setNumThreads(0);

	spb::init_bench(argc, argv); //Initializations

	spb::Metrics::init();

	std::vector<std::unique_ptr<ff::ff_node>> workers;
	for(int i=0; i<spb::nthreads; i++){
		workers.push_back(std::make_unique<Worker>());
	}
	ff::ff_OFarm<spb::Item> farm(move(workers));

	Emitter E;
	farm.add_emitter(E);
	farm.add_collector(Collector);

	farm.set_scheduling_ondemand();

	if(farm.run_and_wait_end()<0){
		std::cout << "error running pipe";
	}

	spb::Metrics::stop();
	spb::end_bench();

	return 0;
}

