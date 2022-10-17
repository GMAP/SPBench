#include <ferret.hpp>
#include <tbb/pipeline.h>
#include "tbb/task_scheduler_init.h"

class Source : public tbb::filter{
public:
    Source() : tbb::filter(tbb::filter::serial_out_of_order) {}
    void* operator() (void*){
        while(1){
            spb::Item * item = new spb::Item();
            if (!spb::Source::op(*item)) break;
            return item;
        }
        return NULL;
    }
};

class Segmentation : public tbb::filter{
public:
    Segmentation() : tbb::filter(tbb::filter::parallel) {}
    void* operator() (void* new_item){
        spb::Item * item = static_cast <spb::Item*> (new_item);
        spb::Segmentation::op(*item);
        return item;
    }
};

class Extract : public tbb::filter{
public:
    Extract() : tbb::filter(tbb::filter::parallel) {}
    void* operator() (void* new_item){
        spb::Item * item = static_cast <spb::Item*> (new_item);
        spb::Extract::op(*item);
        return item;
    }
};

class Vectorization : public tbb::filter{
public:
    Vectorization() : tbb::filter(tbb::filter::parallel) {}
    void* operator() (void* new_item){
        spb::Item * item = static_cast <spb::Item*> (new_item);
        spb::Vectorization::op(*item);
        return item;
    }
};

class Rank : public tbb::filter{
public:
    Rank() : tbb::filter(tbb::filter::parallel) {}
    void* operator() (void* new_item){
        spb::Item * item = static_cast <spb::Item*> (new_item);
        spb::Rank::op(*item);
        return item;
    }
};

class Sink : public tbb::filter{
public:
    Sink() : tbb::filter(tbb::filter::serial_out_of_order) {}
    void* operator() (void* new_item){
    //Token* operator()(Token* t)const{
        spb::Item * item = static_cast <spb::Item*> (new_item);
        spb::Sink::op(*item);
		delete item;
        return NULL;
    }
};

int main(int argc, char *argv[]) {

    spb::init_bench(argc, argv);
    spb::Metrics::init();

    //TBB code
    tbb::task_scheduler_init init_parallel(spb::nthreads);

    tbb::pipeline pipeline;

    Source source;
    pipeline.add_filter(source);
    Segmentation seg;
    pipeline.add_filter(seg);
    Extract ext;
    pipeline.add_filter(ext);
    Vectorization vec;
    pipeline.add_filter(vec);
    Rank rank;
    pipeline.add_filter(rank);
    Sink sink;
    pipeline.add_filter(sink);

    pipeline.run(spb::nthreads*10);
    //END

    spb::Metrics::stop();
    spb::end_bench();
    return 0;
}
