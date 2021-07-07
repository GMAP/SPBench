#include <metrics.hpp>
#include <ferret.hpp>
#include <tbb/pipeline.h>
#include "tbb/task_scheduler_init.h"
using namespace tbb;

class Source : public tbb::filter{
public:
    Source() : tbb::filter(tbb::filter::serial_out_of_order) {}
    void* operator() (void*){
        while(1){
            Item * item = new Item();
            if (!source_op(*item)) break;
            return item;
        }
        return NULL;
    }
};

class Segmentation : public tbb::filter{
public:
    Segmentation() : tbb::filter(tbb::filter::serial_out_of_order) {}
    void* operator() (void* new_item){
        Item * item = static_cast <Item*> (new_item);
        segmentation_op(*item);
        return item;
    }
};

class Extract : public tbb::filter{
public:
    Extract() : tbb::filter(tbb::filter::serial_out_of_order) {}
    void* operator() (void* new_item){
        Item * item = static_cast <Item*> (new_item);
        extract_op(*item);
        return item;
    }
};

class Vectorization : public tbb::filter{
public:
    Vectorization() : tbb::filter(tbb::filter::serial_out_of_order) {}
    void* operator() (void* new_item){
        Item * item = static_cast <Item*> (new_item);
        vectorization_op(*item);
        return item;
    }
};

class Rank : public tbb::filter{
public:
    Rank() : tbb::filter(tbb::filter::serial_out_of_order) {}
    void* operator() (void* new_item){
        Item * item = static_cast <Item*> (new_item);
        rank_op(*item);
        return item;
    }
};

class Sink : public tbb::filter{
public:
    Sink() : tbb::filter(tbb::filter::serial_out_of_order) {}
    void* operator() (void* new_item){
    //Token* operator()(Token* t)const{
        Item * item = static_cast <Item*> (new_item);
        sink_op(*item);
        return NULL;
    }
};

int main(int argc, char *argv[]) {

    init_bench(argc, argv);
    data_metrics metrics = init_metrics();

    //TBB code
    task_scheduler_init init_parallel(nthreads);

    pipeline pipeline;

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

    pipeline.run(nthreads*10);
    //END

    stop_metrics(metrics);
    end_bench();
    return 0;
}
