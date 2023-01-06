#include <fraud_detection.hpp>

Markov_Model_Predictor predictor; // Predictor state

int main (int argc, char* argv[]){
	spb::init_bench(argc, argv); // Initializations
	spb::Metrics::init();
	while(1){
		spb::Item item;
		if(!spb::Source::op(item)) break;
		spb::Predictor::op(item, predictor); 
		if(item) spb::Sink::op(item); // Only fraudulent items proceed (filter)
	}
	spb::Metrics::stop();
	spb::end_bench();
	return 0;
}

