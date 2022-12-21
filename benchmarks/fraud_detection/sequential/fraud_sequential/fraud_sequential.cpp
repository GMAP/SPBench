#include <fraud_detection.hpp>

int main (int argc, char* argv[]){
	spb::init_bench(argc, argv); // Initializations
	spb::Metrics::init();
	while(1){
		spb::Item item;
		if(!spb::Source::op(item)) break;
		spb::Predictor::op(item);
		spb::Sink::op(item);
	}
	spb::Metrics::stop();
	spb::end_bench();
	return 0;
}

