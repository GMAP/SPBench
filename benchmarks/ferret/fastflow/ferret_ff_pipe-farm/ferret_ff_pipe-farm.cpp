/*​
Author: Adriano Vogel
Email: <adriano.vogel@edu.pucrs.br>
July 2022

Modified by Adriano Garcia in July 2022
Email: <adriano.garcia@edu.pucrs.br>
​
Although validated, there are no guarantees that this code will work. In case of questions, please contact via email.
*/

#include <ferret.hpp>
#include <ff/ff.hpp>

struct Source: ff::ff_node_t<spb::Item>{
    Source(){}
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
    Segmentation(){}
    spb::Item * svc(spb::Item * item){
        spb::Segmentation::op(*item);
        return item;
    }
};

struct Extract: ff::ff_node_t<spb::Item>{
    Extract(){}
    spb::Item * svc(spb::Item * item){
        spb::Extract::op(*item);
        return item;
    }
};

struct Vectorization: ff::ff_node_t<spb::Item>{
    Vectorization(){}
    spb::Item * svc(spb::Item * item){
        spb::Vectorization::op(*item);
        return item;
    }
};

struct Rank: ff::ff_node_t<spb::Item>{
    Rank(){}
    spb::Item * svc(spb::Item * item){
        spb::Rank::op(*item);
        return item;
    }
};

struct Sink: ff::ff_minode_t<spb::Item>{
    Sink(){}
    spb::Item * svc(spb::Item * item){
        spb::Sink::op(*item);
		delete item;
        return GO_ON;
    }
};

int main(int argc, char *argv[]) {

    spb::init_bench(argc, argv);

    /* Segmentation stage */
    ff::ff_pipeline *pipeSeg = new ff::ff_pipeline;
    ff::ff_farm farmSeg;
    std::vector<ff::ff_node *> segWorkers;
    for(int i=0;i<spb::nthreads;++i) {
        segWorkers.push_back(new Segmentation());
    }
    farmSeg.add_workers(segWorkers);
    pipeSeg->add_stage(&farmSeg);

    #if defined(DEBUG)
        printf("First Farm: %d threads\n", pipeSeg->cardinality());
    #endif

    /* Extract stage */
    ff::ff_pipeline *pipeExt = new ff::ff_pipeline;
    ff::ff_farm farmExt;
    std::vector<ff::ff_node *> extWorkers;
    for(int i=0;i<spb::nthreads;++i) {
        extWorkers.push_back(new Extract());
    }
    farmExt.add_workers(extWorkers);
    pipeExt->add_stage(&farmExt);

    #if defined(DEBUG)
        printf("Second Farm: %d threads\n", pipeExt->cardinality());
    #endif

    /* Vectorization stage */
    ff::ff_pipeline *pipeVec = new ff::ff_pipeline;
    ff::ff_farm farmVec;
    std::vector<ff::ff_node *> vecWorkers;
    for(int i=0;i<spb::nthreads;++i) {
        vecWorkers.push_back(new Vectorization());
    }
    farmVec.add_workers(vecWorkers);
    pipeVec->add_stage(&farmVec);

    #if defined(DEBUG)
        printf("Third Farm: %d threads\n", pipeVec->cardinality());
    #endif

    /* Rank stage */
    ff::ff_pipeline *pipeRank = new ff::ff_pipeline;
    ff::ff_farm farmRank;
    std::vector<ff::ff_node *> rankWorkers;
    for(int i=0;i<spb::nthreads;++i) {
        rankWorkers.push_back(new Rank());
    }
    farmRank.add_workers(rankWorkers);
    pipeRank->add_stage(&farmRank);

    #if defined(DEBUG)
        printf("Fourth Farm: %d threads\n", pipeRank->cardinality());
    #endif

    Source *source = new Source();
    Sink *sink = new Sink();

    ff::ff_pipeline pipe;
    pipe.add_stage(source);
    pipe.add_stage(pipeSeg);
    pipe.add_stage(pipeExt); 
    pipe.add_stage(pipeVec); 
    pipe.add_stage(pipeRank); 

    pipe.add_stage(sink);

    printf("The total number of threads from the inner composition + the source and Sink is: %d\n", pipe.cardinality());
    
    spb::Metrics::init();

    pipe.run();
    pipe.wait();

    spb::Metrics::stop();
    spb::end_bench();
    return 0;
}
