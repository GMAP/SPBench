#include <lane_detection.hpp>
#include <ff/ff.hpp>

struct Source: ff::ff_node_t<spb::Item>{
	Source(){}
	spb::Item * svc(spb::Item * task){
		while (1){
			spb::Item * item = new spb::Item();
			if (!spb::Source::op(*item)) break;
			ff_send_out(item);
		}
		return EOS;
	}
};

struct Worker1: ff::ff_node_t<spb::Item>{
	Worker1(){}
	spb::Item * svc(spb::Item * item){
		spb::Segment::op(*item);
		return item;
	}
};

struct Worker2: ff::ff_node_t<spb::Item>{
	Worker2(){}
	spb::Item * svc(spb::Item * item){
		spb::Canny1::op(*item);
		spb::HoughT::op(*item);
		spb::HoughP::op(*item);
		spb::Bitwise::op(*item);
		spb::Canny2::op(*item);
		spb::Overlap::op(*item);
		return item;
	}
};

struct Sink: ff::ff_node_t<spb::Item>{
	Sink(){}
	spb::Item * svc(spb::Item * item){
		spb::Sink::op(*item);
		delete item;
		return GO_ON;
	}
};

int main (int argc, char* argv[]){

	// Disabling internal OpenCV's support for multithreading.
	cv::setNumThreads(0);

	spb::init_bench(argc, argv); //Initializations

	std::vector<std::unique_ptr<ff::ff_node>> W_set1;
	for(int i=0; i<spb::nthreads; i++){
		W_set1.push_back(std::make_unique<Worker1>());
	}
	ff::ff_OFarm<spb::Item> farm1(move(W_set1));

#if defined(DEBUG)
	printf("First Farm: %d threads\n", pipeVec->cardinality());
#endif

	std::vector<std::unique_ptr<ff::ff_node>> W_set2;
	for(int i=0; i<spb::nthreads; i++){
		W_set2.push_back(std::make_unique<Worker2>());
	}
	ff::ff_OFarm<spb::Item> farm2(move(W_set2));

#if defined(DEBUG)
	printf("Second Farm: %d threads\n", pipeRank->cardinality());
#endif

	Source source;
	Sink sink;
	farm1.add_emitter(source);
	farm2.add_collector(sink);

	ff::ff_Pipe<> pipe(farm1, farm2);

	printf("The total number of threads from the inner composition + the source and Sink is: %d\n", pipe.cardinality());

	spb::Metrics::init();

	pipe.run();
	pipe.wait();

	spb::Metrics::stop();
	spb::end_bench();
	return 0;
}

