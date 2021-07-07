#include <metrics.hpp>
#include <ferret.hpp>

#include <ff/ff.hpp>

using namespace ff;

struct Source: ff_node_t<Item>{
    Item * svc(Item * task){
        while (1){
            Item * item = new Item();
            if (!source_op(*item)) break;
            ff_send_out(item);
        }
        return EOS;
    }
};

struct Segmentation: ff_node_t<Item>{
    Item * svc(Item * item){
        segmentation_op(*item);
        return item;
    }
};

struct Extract: ff_node_t<Item>{
    Item * svc(Item * item){
        extract_op(*item);
        return item;
    }
};

struct Vectorization: ff_node_t<Item>{
    Item * svc(Item * item){
        vectorization_op(*item);
        return item;
    }
};

struct Rank: ff_node_t<Item>{
    Item * svc(Item * item){
        rank_op(*item);
        return item;
    }
};

struct Sink: ff_node_t<Item>{
    Item * svc(Item * item){
        sink_op(*item);
        return GO_ON;
    }
};

int main(int argc, char *argv[]) {

    init_bench(argc, argv);
    data_metrics metrics = init_metrics();

    ff_Pipe<Item> pipe(
        new Source(),
        new Segmentation(),
        new Extract(),
        new Vectorization(),
        new Rank(),
        new Sink());

    if (pipe.run_and_wait_end()<0) error("running pipe");

    stop_metrics(metrics);
    end_bench();
    return 0;
}
