#include <metrics.hpp>
#include <ferret.hpp>

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
        segmentation_op(*item);
        extract_op(*item);
        vectorization_op(*item);
        rank_op(*item);
        return item;
    }
};

struct Collector: ff_node_t<Item>{
    Item * svc(Item * item){
        sink_op(*item);
        return GO_ON;
    }
}Collector;

int main(int argc, char *argv[]) {

    init_bench(argc, argv);
    data_metrics metrics = init_metrics();
    vector<unique_ptr<ff_node>> workers;

    for(int i=0; i<nthreads; i++){
        workers.push_back(make_unique<Worker>());
    }

    ff_Farm<Item> farm(move(workers));

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
