#include <ferret.hpp>

using namespace spb;

Source source;
Segmentation segmentation;
Extract extract;
Vectorization vectorization;
Rank rank;
Sink sink;

int main(int argc, char *argv[]) {

    init_bench(argc, argv);
    Metrics::init();

    [[spar::ToStream]]
    while(1) {
        Item item;
        if(!source.op(item)) break;
        [[spar::Stage,spar::Input(item),spar::Output(item),spar::Replicate()]]
        {
            segmentation.op(item);
            extract.op(item);
            vectorization.op(item);
            rank.op(item);
        }      
        [[spar::Stage,spar::Input(item)]]
        {
            sink.op(item);
        }
    }
    Metrics::stop();
    end_bench();
    return 0;
}

