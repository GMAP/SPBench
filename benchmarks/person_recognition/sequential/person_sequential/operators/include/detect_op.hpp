#include <person_recognition.hpp>

namespace spb{
void Detect::op(Item &item){


	#if !defined NO_LATENCY
		Metrics metrics;
		std::chrono::high_resolution_clock::time_point op_timestamp1;
		if(Metrics::latency_is_enabled()){
			op_timestamp1 = std::chrono::high_resolution_clock::now();
		}
	#endif
	unsigned int num_item = 0;

	while(num_item < item.batch_size){ //batch loop

		detect_op(item.item_batch[num_item]);

		num_item++;
	}
	
	if(metrics.latency_is_enabled()){
		//item.latency_op.push_back(current_time_usecs() - latency_op);
		item.latency_op.push_back(current_time_usecs() - latency_op);
	}
}

}