#include <fraud_detection.hpp>
#include <windflow.hpp>
#include <ff/ff.hpp>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <iostream>

using namespace wf;
using namespace ff;
using namespace std;


// Source_Functor class
class Source_Functor{
public:
    // Constructor
    Source_Functor() {}

    // operator() method
    void operator()(Source_Shipper<spb::Item> &shipper){
        while (1){ // generation loop
			spb::Item item;
			if(!spb::Source::op(item)) break;
            shipper.push(std::move(item)); // send the next tuple
        }
    }
    // Destructor
    ~Source_Functor(){}
};

// Predictor_Functor class
class Predictor_Functor{
private: 
	Markov_Model_Predictor predictor; // predictor state
public:
    // Constructor
    Predictor_Functor(){}

    // operator() method
    bool operator()(spb::Item &item){
        spb::Predictor::op(item, predictor);
		if(item) return true;
		return false;
    }
    // Destructor
    ~Predictor_Functor(){}
};

// Sink_Functor class
class Sink_Functor{
public:
    // Constructor
    Sink_Functor(){}

    // operator() method
    void operator()(optional<spb::Item> &item){
		spb::Sink::op(*item);
    }
	// Destructor
	~Sink_Functor(){}
};

int main (int argc, char* argv[]){
	spb::init_bench(argc, argv); // Initializations
	
	const string topology_name = "FraudDetection";

    PipeGraph topology(topology_name, Execution_Mode_t::DEFAULT, Time_Policy_t::INGRESS_TIME);

	Source_Functor source_functor;
	Source source = Source_Builder(source_functor)
						.withParallelism(1)
						.withName("Source")
						.withOutputBatchSize(1)
						.build();
	Predictor_Functor predictor_functor;
	Filter predictor = Filter_Builder(predictor_functor)
							.withParallelism(spb::nthreads)
							.withName("Predictor")
							.withKeyBy([](const spb::Item &item) -> size_t { return item.key; })
							//.withOutputBatchSize(1)
							.build();
	Sink_Functor sink_functor;
	Sink sink = Sink_Builder(sink_functor)
					.withParallelism(1)
					.withName("Sink")
					.build();
	/// create the application
	MultiPipe &mp = topology.add_source(source);
	cout << "Chaining is disabled" << endl;
	mp.add(predictor);
	mp.add_sink(sink);

	spb::Metrics::init();

	topology.run();

	spb::Metrics::stop();

	spb::end_bench();
	return 0;
}

