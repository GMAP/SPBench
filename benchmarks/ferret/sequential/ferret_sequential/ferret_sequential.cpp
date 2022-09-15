#include <ferret.hpp>

int main(int argc, char *argv[]) {

    spb::init_bench(argc, argv);
	spb::Metrics::init();

    while(1) {
        spb::Item item;
        if(!spb::Source::op(item)) break;
        spb::Segmentation::op(item);
        spb::Extract::op(item);
        spb::Vectorization::op(item);
        spb::Rank::op(item);
        spb::Sink::op(item);
    }
    spb::Metrics::stop();
	spb::end_bench();
    return 0;
}