#include <bzip2.hpp>

namespace spb{
void Decompress::op(Item &item){

	if(item.empty())
		return;


	#if !defined NO_LATENCY
		Metrics metrics;
		std::chrono::high_resolution_clock::time_point op_timestamp1;
		if(Metrics::latency_is_enabled()){
			op_timestamp1 = std::chrono::high_resolution_clock::now();
		}
	#endif
	unsigned int num_item = 0;

	while(num_item < item.batch_size){ //batch loop

		decompress_op(item.item_batch[num_item]);

		num_item++;
	}
	
	#if !defined NO_LATENCY
		if(metrics.latency_is_enabled()){
			std::chrono::high_resolution_clock::time_point op_timestamp2 = std::chrono::high_resolution_clock::now();
			item.latency_op.push_back(std::chrono::duration_cast<std::chrono::duration<double>>(op_timestamp2 - op_timestamp1));
		}
	#endif
}

}