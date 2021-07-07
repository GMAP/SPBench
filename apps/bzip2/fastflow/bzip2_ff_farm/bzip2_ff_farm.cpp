#include <metrics.hpp>
#include <bzip2.hpp>
#include <ff/ff.hpp>
using namespace ff;
using namespace std;

struct Emitter_comp: ff_node_t<Item>{
	Item * svc(Item * task){
		while (bytesLeft > 0){
			Item * item = new Item();
			if (!read_comp_op(*item)) break;
		    ff_send_out(item);
		}
		return EOS;
	}
};

struct Worker_comp: ff_node_t<Item>{
	Item * svc(Item * item){
		compress_op(*item);
		return item;
	}
};

struct Collector_comp: ff_node_t<Item>{
	Item * svc(Item * item){
		write_comp_op(*item);
		return GO_ON;
	}
}Collector_comp;


void compress(){

	data_metrics metrics = init_metrics();

	/*--------FastFlow region-------*/

	vector<unique_ptr<ff_node>> workers;

	for(int i=0; i<nthreads; i++){
		workers.push_back(make_unique<Worker_comp>());
	}
	ff_OFarm<Item> farm(move(workers));
	Emitter_comp E;
	farm.add_emitter(E);
	farm.add_collector(Collector_comp);

	farm.set_scheduling_ondemand();

	if(farm.run_and_wait_end()<0){
		cout << "error running pipe";
	}

	/*------------------------------*/

	stop_metrics(metrics);
}

struct Emitter_decomp: ff_node_t<Item>{
	Item * svc(Item * task){
		while (item_count < bz2NumBlocks){
			Item * item = new Item();
			if (!read_decomp_op(*item)) break;
		    ff_send_out(item);
		}
		return EOS;
	}
};

struct Worker_decomp: ff_node_t<Item>{
	Item * svc(Item * item){
		decompress_op(*item);
		return item;
	}
};

struct Collector_decomp: ff_node_t<Item>{
	Item * svc(Item * item){
		write_decomp_op(*item);
		return GO_ON;
	}
}Collector_decomp;

void decompress(){

	data_metrics metrics = init_metrics();

	/*--------FastFlow region-------*/

	vector<unique_ptr<ff_node>> workers;

	for(int i=0; i<nthreads; i++){
		workers.push_back(make_unique<Worker_decomp>());
	}

	ff_OFarm<Item> farm(move(workers));

	Emitter_decomp E;
	farm.add_emitter(E);
	farm.add_collector(Collector_decomp);

	farm.set_scheduling_ondemand();

	if(farm.run_and_wait_end()<0){
		cout << "error running pipe";
	}
	/*------------------------------*/
	stop_metrics(metrics);
}

int main (int argc, char* argv[]){
	bzip2_main(argc, argv);
	return 0;
}
