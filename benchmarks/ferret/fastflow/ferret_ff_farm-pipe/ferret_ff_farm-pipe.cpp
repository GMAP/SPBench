/*
Author: Adriano Marques Garcia
Email: <adriano.garcia@edu.pucrs.br>
November 2022

Although validated, there are no guarantees that this code will work.
In case of questions, please contact via email.
*/

#include <ferret.hpp>
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

struct Segmentation: ff::ff_node_t<spb::Item>{
	Segmentation(){}
	spb::Item * svc(spb::Item * item){
		spb::Segmentation::op(*item);
		return item;
	}
};

struct Extract: ff::ff_node_t<spb::Item>{
	Extract(){}
	spb::Item * svc(spb::Item * item){
		spb::Extract::op(*item);
		return item;
	}
};

struct Vectorization: ff::ff_node_t<spb::Item>{
	Vectorization(){}
	spb::Item * svc(spb::Item * item){
		spb::Vectorization::op(*item);
		return item;
	}
};

struct Rank: ff::ff_node_t<spb::Item>{
	Rank(){}
	spb::Item * svc(spb::Item * item){
		spb::Rank::op(*item);
		return item;
	}
};

struct Sink: ff::ff_minode_t<spb::Item>{
	Sink(){}
	spb::Item * svc(spb::Item * item){
		spb::Sink::op(*item);
		delete item;
		return GO_ON;
	}
};

int main(int argc, char *argv[]) {

	spb::init_bench(argc, argv);

	ff::ff_farm farm;
	Source source;
	Sink sink;
	farm.add_emitter(&source);
	farm.add_collector(&sink);

	std::vector<ff::ff_node *> w;
	for(int i=0;i<spb::nthreads;++i) {
		// build worker pipeline 
		ff::ff_pipeline * pipe = new ff::ff_pipeline;
		pipe->add_stage(new Segmentation, true);
		pipe->add_stage(new Extract, true);
		pipe->add_stage(new Vectorization, true);
		pipe->add_stage(new Rank, true);
		w.push_back(pipe);
	}
	farm.add_workers(w);

	farm.cleanup_workers();

	printf("The total number of threads from the inner composition + the source and Sink is: %d\n", farm.cardinality());

	spb::Metrics::init();

	farm.run();
	farm.wait();

	spb::Metrics::stop();
	spb::end_bench();
	return 0;
}
