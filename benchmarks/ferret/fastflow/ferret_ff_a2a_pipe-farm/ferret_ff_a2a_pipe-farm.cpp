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

struct Source: ff::ff_monode_t<spb::Item> { 
    Source(){}
    spb::Item *svc(spb::Item*) {
        while (1){
            spb::Item * item = new spb::Item();
            if (!spb::Source::op(*item)) break;
            ff_send_out(item);
        }
        return EOS;
    }
};

struct Segmentation: ff::ff_monode_t<spb::Item>{
    Segmentation(){}
    spb::Item * svc(spb::Item * item){
        spb::Segmentation::op(*item);
        return item;
    }
};

struct Extract: ff::ff_monode_t<spb::Item>{
    Extract(){}
    spb::Item * svc(spb::Item * item){
        spb::Extract::op(*item);
        return item;
    }
};

struct Vectorization: ff::ff_monode_t<spb::Item>{
    Vectorization(){}
    spb::Item * svc(spb::Item * item){
        spb::Vectorization::op(*item);
        return item;
    }
};

struct Rank: ff::ff_monode_t<spb::Item>{
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

struct miHelper: ff::ff_minode_t<spb::Item> {
    spb::Item* svc(spb::Item* item){
    return item;
    }
};

struct moHelper: ff::ff_monode_t<spb::Item> {
    spb::Item* svc(spb::Item* item){
    return item;
    }
};

int main(int argc, char** argv) {

    spb::init_bench(argc, argv);

   
    std::vector<ff::ff_node*> W_1a, W_1b, W_1c, W_1d, W_1e;

    ff::ff_a2a a2a_internal_1a, a2a_internal_1b;
    for(long i=0;i<spb::nthreads;++i)
    {
        W_1a.push_back(new ff::ff_comb(new miHelper, new Segmentation()));
        W_1b.push_back(new ff::ff_comb(new miHelper, new Extract()));
        W_1c.push_back(new ff::ff_comb(new miHelper, new Vectorization()));
        W_1d.push_back(new ff::ff_comb(new miHelper, new Rank()));
    }

    a2a_internal_1a.add_firstset(W_1a);
    a2a_internal_1a.add_secondset(W_1b);

    a2a_internal_1b.add_firstset(W_1c);
    a2a_internal_1b.add_secondset(W_1d);
    
    //Sink col();

    W_1a.clear(),W_1b.clear(),W_1c.clear(),W_1d.clear(),W_1e.clear();

    Source *source = new Source();
    Sink *sink = new Sink();

    ff::ff_Pipe<> pipe(source, a2a_internal_1a, a2a_internal_1b, sink);

    printf("The total number of threads from the inner composition + the source and Sink is: %d\n", pipe.cardinality());

    spb::Metrics::init();
   
    pipe.run();
    pipe.wait();

    spb::Metrics::stop();
    spb::end_bench();

    return 0;
}