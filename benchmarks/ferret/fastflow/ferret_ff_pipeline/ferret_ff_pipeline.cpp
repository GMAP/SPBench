#include <ferret.hpp>

#include <ff/ff.hpp>

struct Source: ff::ff_node_t<spb::Item>{
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
    spb::Item * svc(spb::Item * item){
        spb::Segmentation::op(*item);
        return item;
    }
};

struct Extract: ff::ff_node_t<spb::Item>{
    spb::Item * svc(spb::Item * item){
        spb::Extract::op(*item);
        return item;
    }
};

struct Vectorization: ff::ff_node_t<spb::Item>{
    spb::Item * svc(spb::Item * item){
        spb::Vectorization::op(*item);
        return item;
    }
};

struct Rank: ff::ff_node_t<spb::Item>{
    spb::Item * svc(spb::Item * item){
        spb::Rank::op(*item);
        return item;
    }
};

struct Sink: ff::ff_node_t<spb::Item>{
    spb::Item * svc(spb::Item * item){
        spb::Sink::op(*item);
		delete item;
        return GO_ON;
    }
};

int main(int argc, char *argv[]) {

    spb::init_bench(argc, argv);
    spb::Metrics::init();

    ff::ff_Pipe<spb::Item> pipe(
        new Source(),
        new Segmentation(),
        new Extract(),
        new Vectorization(),
        new Rank(),
        new Sink());

    if (pipe.run_and_wait_end()<0) ff::error("running pipe");

    spb::Metrics::stop();
    spb::end_bench();
    return 0;
}
