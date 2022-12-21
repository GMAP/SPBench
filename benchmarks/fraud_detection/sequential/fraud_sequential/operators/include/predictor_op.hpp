#include <fraud_detection.hpp>

namespace spb{

void Predictor::op(Item &item){

	Metrics metrics;
	volatile unsigned long latency_op;
	if(metrics.latency_is_enabled()){
		latency_op = current_time_usecs();
	}
	unsigned int num_item = 0;

	while(num_item < item.batch_size){ //batch loop

		predictor_op(item.item_batch[num_item]);

		num_item++;
	}
	
	if(metrics.latency_is_enabled()){
		item.latency_op.push_back(current_time_usecs() - latency_op);
	}
}

}