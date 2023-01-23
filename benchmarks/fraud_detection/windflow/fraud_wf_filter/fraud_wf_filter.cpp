#include <fraud_detection.hpp>
#include <windflow.hpp>
#include <ff/ff.hpp>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <iostream>

// Source_Functor class
class Source_Functor{
private:
    long generated_tuples;
public:
    Source_Functor() {
		generated_tuples = 0;
	}
    void operator()(wf::Source_Shipper<spb::Item> &shipper){
        while (1){ // generation loop
			spb::Item item;
			if(!spb::Source::op(item)) break;
            shipper.push(std::move(item));
			generated_tuples++;
        }
		spb::Metrics::my_items_at_source_counter.fetch_add(generated_tuples);
    }
    ~Source_Functor(){}
};

// Predictor_Functor class
class Predictor_Functor{
private: 
	Markov_Model_Predictor predictor; // predictor state
public:
    Predictor_Functor(){}
    bool operator()(spb::Item &item){
        spb::Predictor::op(item, predictor);
		if(item) return true;
		return false;
    }
    ~Predictor_Functor(){}
};

// Sink_Functor class
class Sink_Functor{
public:
    Sink_Functor(){}
    void operator()(optional<spb::Item> &item){
		spb::Sink::op(*item);
    }
	~Sink_Functor(){}
};

int main (int argc, char* argv[]){
	spb::init_bench(argc, argv); // Initializations
	
	const std::string topology_name = "FraudDetection";

    wf::PipeGraph topology(topology_name, wf::Execution_Mode_t::DEFAULT, wf::Time_Policy_t::INGRESS_TIME);

	Source_Functor source_functor;
	wf::Source source = wf::Source_Builder(source_functor)
						.withParallelism(stoi(spb::SPBench::getArg(0)))
						.withName("Source")
						.withOutputBatchSize(1)
						.build();
	Predictor_Functor predictor_functor;
	wf::Filter predictor = wf::Filter_Builder(predictor_functor)
							.withParallelism(spb::nthreads)
							.withName("Predictor")
							.withKeyBy([](const spb::Item &item) -> size_t { return item.key; })
							//.withOutputBatchSize(1)
							.build();
	Sink_Functor sink_functor;
	wf::Sink sink = wf::Sink_Builder(sink_functor)
					.withParallelism(1)
					.withName("Sink")
					.build();

	// create the application
	wf::MultiPipe &mp = topology.add_source(source);
	std::cout << " Chaining is disabled" << std::endl;
	mp.add(predictor);
	mp.add_sink(sink);

	spb::Metrics::init();

	topology.run();

	spb::Metrics::stop();

	spb::end_bench();
	return 0;
}

