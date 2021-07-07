#include <metrics.hpp>
#include <ferret.hpp>

#include <ff/ff.hpp>
#include <ff/pipeline.hpp>
 
#include <ff/farm.hpp>

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
    

    /*vector<unique_ptr<ff_node>> workers;

    for(int i=0; i<nthreads; i++){
        workers.push_back(make_unique<Worker>());
    }

    ff_OFarm<Item> farm(move(workers));

    Emitter E;
    farm.add_emitter(E);
    farm.add_collector(Collector);

    farm.set_scheduling_ondemand();

    if(farm.run_and_wait_end()<0){
        cout << "error running pipe";
    }

    ff::ff_pipeline p;
    vector<unique_ptr<ff_node>> segW, extW, vecW, rankW;

    for(size_t i = 0; i < nthreads; i++){
        segW.push_back(make_unique<Segmentation>());
        extW.push_back(make_unique<Extract>());
        vecW.push_back(make_unique<Vectorization>());
        rankW.push_back(make_unique<Rank>());
    }

 //   ff_Farm<Item> segFarm(move(segW)), extFarm(move(extW)), vecFarm(move(vecW)), rankFarm(move(rankW));

    ff_Farm<Item> segFarm(move(segW));
    ff_Farm<Item> extFarm(move(extW));
    ff_Farm<Item> vecFarm(move(vecW));
    ff_Farm<Item> rankFarm(move(rankW));


    segFarm.set_scheduling_ondemand();
    extFarm.set_scheduling_ondemand();
    vecFarm.set_scheduling_ondemand();
    rankFarm.set_scheduling_ondemand();

    ff_Pipe<Item> pipe(
        new Source(), 
        segFarm, 
        extFarm,
        vecFarm,
        rankFarm,
        new Sink());

    if (pipe.run_and_wait_end()<0) error("running pipe"); 

    segFarm.add_emitter(new Load());
    segFarm.add_workers(segW);
    extFarm.add_workers(extW);
    vecFarm.add_workers(vecW);
    rankFarm.add_workers(rankW);
    segFarm.remove_collector();
    extFarm.setMultiInput();
    extFarm.remove_collector();
    vecFarm.setMultiInput();
    vecFarm.remove_collector();
    rankFarm.setMultiInput();
    rankFarm.add_collector(new Out());

    segFarm.set_scheduling_ondemand();
    extFarm.set_scheduling_ondemand();
    vecFarm.set_scheduling_ondemand();
    rankFarm.set_scheduling_ondemand();

    p.add_stage(&segFarm);
    p.add_stage(&extFarm);
    p.add_stage(&vecFarm);
    p.add_stage(&rankFarm);

    ff_Pipe<Item> pipe(
        new Source(), 
        new Segmentation(), 
        new Extract(),
        new Vectorization(),
        new Rank(),
        new Sink());
*/

    
    std::vector <std::unique_ptr <ff_node>> segW (nthreads); 
    
    for(size_t i = 0; i < nthreads; i++){
        segW[i] = std::unique_ptr <ff_node>(new Segmentation); 
    }

    std::vector <std::unique_ptr <ff_node>> extW (nthreads);
    for(size_t i = 0; i < nthreads; i++){
        extW[i] = std::unique_ptr <ff_node>(new Extract); 
    }
    
    std::vector < std::unique_ptr <ff_node>> vecW (nthreads); 
    for(size_t i = 0; i < nthreads; i++){
        vecW[i] = std::unique_ptr <ff_node>(new Vectorization); 
    }
    
    std::vector < std::unique_ptr <ff_node>> rankW (nthreads);
    for(size_t i = 0; i < nthreads; i++){
        rankW[i] = std::unique_ptr <ff_node>(new Rank); 
    }

    Source source;
    Sink sink;

    ff_Farm <Item> segFarm (std::move(segW));
    segFarm.add_emitter(source);

    ff_Farm <Item> extFarm (std::move(extW)); 
    ff_Farm <Item> vecFarm (std::move(vecW)); 
    
    ff_Farm <Item> rankFarm (std::move(rankW));
    rankFarm.add_collector(sink);

    segFarm.set_scheduling_ondemand(); 
    extFarm.set_scheduling_ondemand(); 
    vecFarm.set_scheduling_ondemand(); 
    rankFarm.set_scheduling_ondemand();

    ff_Pipe <Item> pipeline(segFarm, extFarm, vecFarm, rankFarm);

    if(pipeline.run_and_wait_end() < 0)
    {
        error("Running pipeline\n"); 
        exit(1);
    } 

    stop_metrics(metrics);
    end_bench();
    return 0;
}
/*
#include <metrics.hpp>
 
#include <ferret.hpp>
 
#include <ff/ff.hpp>
 
#include <ff/pipeline.hpp>
 
#include <ff/farm.hpp>
 
using namespace ff; 
namespace spar{
    static inline ssize_t get_mac_core() {
        ssize_t n = 1; 
        FILE * f; 
        f = popen("cat /proc/cpuinfo |grep processor | wc -l","r"); 
        if(fscanf(f,"%ld",& n) == EOF)
        {
            pclose (f); 
            return n;
        } 
        pclose (f); 
        return n;
    } 
    static inline ssize_t get_env_num_workers() {
        ssize_t n = 1; 
        FILE * f; 
        f = popen("echo $SPAR_NUM_WORKERS","r"); 
        if(fscanf(f,"%ld",& n) == EOF)
        {
            pclose (f); 
            return n;
        } 
        pclose (f); 
        return n;
    } 
    static inline ssize_t get_Num_Workers() {
        ssize_t w_size = get_env_num_workers(); 
        if(w_size > 0)
        {
            return w_size;
        } 
        return get_mac_core();
    }
} 
struct _struct_spar0{
    _struct_spar0() {
    } 
    _struct_spar0(Item item) : item(item) {
    } 
    ; 
    Item item;
}; 
struct _Stage_spar00 : ff_node_t < _struct_spar0 >{
    Item item; 
    _struct_spar0 * svc(_struct_spar0 * _Input_spar) {
        {
            segmentation_op(_Input_spar -> item);
        } 
        return _Input_spar;
    }
}; 
struct _Stage_spar01 : ff_node_t < _struct_spar0 >{
    Item item; 
    _struct_spar0 * svc(_struct_spar0 * _Input_spar) {
        {
            extract_op(_Input_spar -> item);
        } 
        return _Input_spar;
    }
}; 
struct _Stage_spar02 : ff_node_t < _struct_spar0 >{
    Item item; 
    _struct_spar0 * svc(_struct_spar0 * _Input_spar) {
        {
            vectorization_op(_Input_spar -> item);
        } 
        return _Input_spar;
    }
}; 
struct _Stage_spar03 : ff_node_t < _struct_spar0 >{
    Item item; 
    _struct_spar0 * svc(_struct_spar0 * _Input_spar) {
        {
            rank_op(_Input_spar -> item);
        } 
        return _Input_spar;
    }
}; 
struct _Stage_spar04 : ff_node_t < _struct_spar0 >{
    _struct_spar0 * svc(_struct_spar0 * _Input_spar) {
        {
            sink_op(_Input_spar -> item);
        } 
        delete _Input_spar; 
        return (_struct_spar0 *)GO_ON;
    }
}; 
struct _ToStream_spar0 : ff_node_t < _struct_spar0 >{
    _struct_spar0 * svc(_struct_spar0 * _Input_spar) {
        
        while(1)
        {
            Item item; 
            if(! source_op(item))
            break; 
            _struct_spar0 * stream_spar = new _struct_spar0 (item); 
            ff_send_out (stream_spar); 
            ; 
            ; 
            ; 
            ;
        } 
        return EOS;
    }
}; 
int main(int argc,char * argv[]) {
    _ToStream_spar0 _ToStream_spar0_call; 
    std::vector < std::unique_ptr < ff_node > > _Stage_spar00_workers (spar::get_Num_Workers()); 
    
    for(unsigned int _Stage_spar00_i = 0; _Stage_spar00_i < spar::get_Num_Workers();++_Stage_spar00_i)
    _Stage_spar00_workers[_Stage_spar00_i] = std::unique_ptr < ff_node >(new _Stage_spar00); 
    ff_Farm < _struct_spar0 > _Stage_spar00_call (std::move(_Stage_spar00_workers)); 
    _Stage_spar00_call.set_scheduling_ondemand(); 
    _Stage_spar00_call.add_emitter(_ToStream_spar0_call); 
    std::vector < std::unique_ptr < ff_node > > _Stage_spar01_workers (spar::get_Num_Workers()); 
    
    for(unsigned int _Stage_spar01_i = 0; _Stage_spar01_i < spar::get_Num_Workers();++_Stage_spar01_i)
    _Stage_spar01_workers[_Stage_spar01_i] = std::unique_ptr < ff_node >(new _Stage_spar01); 
    ff_Farm < _struct_spar0 > _Stage_spar01_call (std::move(_Stage_spar01_workers)); 
    _Stage_spar01_call.set_scheduling_ondemand(); 
    std::vector < std::unique_ptr < ff_node > > _Stage_spar02_workers (spar::get_Num_Workers()); 
    
    for(unsigned int _Stage_spar02_i = 0; _Stage_spar02_i < spar::get_Num_Workers();++_Stage_spar02_i)
    _Stage_spar02_workers[_Stage_spar02_i] = std::unique_ptr < ff_node >(new _Stage_spar02); 
    ff_Farm < _struct_spar0 > _Stage_spar02_call (std::move(_Stage_spar02_workers)); 
    _Stage_spar02_call.set_scheduling_ondemand(); 
    std::vector < std::unique_ptr < ff_node > > _Stage_spar03_workers (spar::get_Num_Workers()); 
    
    for(unsigned int _Stage_spar03_i = 0; _Stage_spar03_i < spar::get_Num_Workers();++_Stage_spar03_i)
    _Stage_spar03_workers[_Stage_spar03_i] = std::unique_ptr < ff_node >(new _Stage_spar03); 
    ff_Farm < _struct_spar0 > _Stage_spar03_call (std::move(_Stage_spar03_workers)); 
    _Stage_spar03_call.set_scheduling_ondemand(); 
    _Stage_spar04 _Stage_spar04_call; 
    _Stage_spar03_call.add_collector(_Stage_spar04_call); 
    ff_Pipe < _struct_spar0 > pipeline0(_Stage_spar00_call,_Stage_spar01_call,_Stage_spar02_call,_Stage_spar03_call); 
    init_bench(argc,argv); 
    data_metrics metrics = init_metrics(); 
    if(pipeline0.run_and_wait_end() < 0)
    {
        error("Running pipeline\n"); 
        exit(1);
    } 
    stop_metrics (metrics); 
    end_bench(); 
    return 0;
}*/