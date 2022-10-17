#include <bzip2.hpp>
#include <ff/ff.hpp>

struct Emitter_comp: ff::ff_node_t<spb::Item>{
	spb::Item * svc(spb::Item * task){
		while (1){
			spb::Item * item = new spb::Item();
			if (!spb::Source::op(*item)) break;
		    ff_send_out(item);
		}
		return EOS;
	}
};

struct Worker_comp: ff::ff_node_t<spb::Item>{
	spb::Item * svc(spb::Item * item){
		spb::Compress::op(*item);
		return item;
	}
};

struct Collector_comp: ff::ff_node_t<spb::Item>{
	spb::Item * svc(spb::Item * item){
		spb::Sink::op(*item);
		delete item;
		return GO_ON;
	}
}Collector_comp;


void compress(){

	spb::Metrics::init();

	/*--------FastFlow region-------*/

	std::vector<std::unique_ptr<ff::ff_node>> workers;

	for(int i=0; i<spb::nthreads; i++){
		workers.push_back(ff::make_unique<Worker_comp>());
	}
	ff::ff_OFarm<spb::Item> farm(move(workers));
	Emitter_comp E;
	farm.add_emitter(E);
	farm.add_collector(Collector_comp);

	farm.set_scheduling_ondemand();

	if(farm.run_and_wait_end()<0){
		std::cout << "error running pipe";
	}

	/*------------------------------*/

	spb::Metrics::stop();
}

struct Emitter_decomp: ff::ff_node_t<spb::Item>{
	spb::Item * svc(spb::Item * task){
		while (1){
			spb::Item * item = new spb::Item();
			if (!spb::Source_d::op(*item)) break;
		    ff_send_out(item);
		}
		return EOS;
	}
};

struct Worker_decomp: ff::ff_node_t<spb::Item>{
	spb::Item * svc(spb::Item * item){
		spb::Decompress::op(*item);
		return item;
	}
};

struct Collector_decomp: ff::ff_node_t<spb::Item>{
	spb::Item * svc(spb::Item * item){
		spb::Sink_d::op(*item);
		return GO_ON;
	}
}Collector_decomp;

void decompress(){

	spb::Metrics::init();

	/*--------FastFlow region-------*/

	std::vector<std::unique_ptr<ff::ff_node>> workers;

	for(int i=0; i<spb::nthreads; i++){
		workers.push_back(ff::make_unique<Worker_decomp>());
	}

	ff::ff_OFarm<spb::Item> farm(move(workers));

	Emitter_decomp E;
	farm.add_emitter(E);
	farm.add_collector(Collector_decomp);

	farm.set_scheduling_ondemand();

	if(farm.run_and_wait_end()<0){
		std::cout << "error running pipe";
	}
	/*------------------------------*/
	spb::Metrics::stop();
}

int main (int argc, char* argv[]){
	spb::bzip2_main(argc, argv);
	return 0;
}
