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
        return GO_ON;
    }
};

int main(int argc, char *argv[]) {

    spb::init_bench(argc, argv);
    spb::Metrics::init();
        
    std::vector <std::unique_ptr <ff::ff_node>> segW (spb::nthreads); 
    
    for(size_t i = 0; i < spb::nthreads; i++){
        segW[i] = std::unique_ptr <ff::ff_node>(new Segmentation); 
    }

    std::vector <std::unique_ptr <ff::ff_node>> extW (spb::nthreads);
    for(size_t i = 0; i < spb::nthreads; i++){
        extW[i] = std::unique_ptr <ff::ff_node>(new Extract); 
    }
    
    std::vector < std::unique_ptr <ff::ff_node>> vecW (spb::nthreads); 
    for(size_t i = 0; i < spb::nthreads; i++){
        vecW[i] = std::unique_ptr <ff::ff_node>(new Vectorization); 
    }
    
    std::vector < std::unique_ptr <ff::ff_node>> rankW (spb::nthreads);
    for(size_t i = 0; i < spb::nthreads; i++){
        rankW[i] = std::unique_ptr <ff::ff_node>(new Rank); 
    }

    Source source;
    Sink sink;

    ff::ff_Farm <spb::Item> segFarm (std::move(segW));
    segFarm.add_emitter(source);

    ff::ff_Farm <spb::Item> extFarm (std::move(extW)); 
    ff::ff_Farm <spb::Item> vecFarm (std::move(vecW)); 
    
    ff::ff_Farm <spb::Item> rankFarm (std::move(rankW));
    rankFarm.add_collector(sink);

    segFarm.set_scheduling_ondemand(); 
    extFarm.set_scheduling_ondemand(); 
    vecFarm.set_scheduling_ondemand(); 
    rankFarm.set_scheduling_ondemand();

    ff::ff_Pipe <spb::Item> pipeline(segFarm, extFarm, vecFarm, rankFarm);

    if(pipeline.run_and_wait_end() < 0)
    {
        ff::error("Running pipeline\n"); 
        exit(1);
    } 

    spb::Metrics::stop();
    spb::end_bench();
    return 0;
}
