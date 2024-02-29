#include <fraud_detection.hpp>

namespace spb{

void Predictor::op(Item &item, Markov_Model_Predictor &predictor){

	Metrics metrics;
	std::chrono::high_resolution_clock::time_point op_timestamp1;
	if(Metrics::latency_is_enabled()){
  		op_timestamp1 = std::chrono::high_resolution_clock::now();
	}
	
	predictor_op(item, predictor);

	if(metrics.latency_is_enabled()){
		std::chrono::high_resolution_clock::time_point op_timestamp2 = std::chrono::high_resolution_clock::now();
		item.latency_op.push_back(std::chrono::duration_cast<std::chrono::duration<double>>(op_timestamp2 - op_timestamp1));
	}
}

}