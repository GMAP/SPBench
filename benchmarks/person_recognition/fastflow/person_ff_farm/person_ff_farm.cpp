#include <person_recognition.hpp>

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
		//detect faces in the image:
		spb::Detect::op(*item);
		
		//analyze each detected face:
		spb::Recognize::op(*item);
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
	//Disabling internal OpenCV's support for multithreading. 
	cv::setNumThreads(0);

	spb::init_bench(argc, argv);
	
	spb::Metrics::init();

	std::vector<std::unique_ptr<ff::ff_node>> workers;

	for(int i=0; i<spb::nthreads; i++){
		workers.push_back(ff::make_unique<Worker>());
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

