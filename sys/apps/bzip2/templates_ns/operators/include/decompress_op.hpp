#include <bzip2.hpp>

namespace spb{

void Decompress::op(Item &item){

	if(item.empty())
		return;

	Metrics metrics;
	volatile unsigned long latency_op;
	if(metrics.latency_is_enabled()){
		latency_op = current_time_usecs();
	}
	unsigned int num_item = 0;

	while(num_item < item.batch_size){ //batch loop

		decompress_op(item.item_batch[num_item]);

		num_item++;
	}
	
	if(metrics.latency_is_enabled()){
		item.latency_op[1] = (current_time_usecs() - latency_op);
	}
}

}