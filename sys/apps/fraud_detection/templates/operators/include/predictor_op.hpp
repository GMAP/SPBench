#include <fraud_detection.hpp>

namespace spb{

void Predictor::op(Item &item, Markov_Model_Predictor &predictor){

	Metrics metrics;
	volatile unsigned long latency_op;
	if(metrics.latency_is_enabled()){
		latency_op = current_time_usecs();
	}
	
	predictor_op(item, predictor);

	if(metrics.latency_is_enabled()){
		item.latency_op.push_back(current_time_usecs() - latency_op);
	}
}

}