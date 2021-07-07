#include <metrics.hpp>
#include <ferret.hpp>

int main(int argc, char *argv[]) {

    init_bench(argc, argv);
    data_metrics metrics = init_metrics();

    while(1) {
        Item item;
        if(!source_op(item)) break;
        segmentation_op(item);
        extract_op(item);
        vectorization_op(item);
        rank_op(item);
        sink_op(item);
    }
    stop_metrics(metrics);
    end_bench();
    return 0;
}
