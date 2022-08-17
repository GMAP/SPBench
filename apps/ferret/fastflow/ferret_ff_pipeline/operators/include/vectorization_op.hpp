#include <ferret.hpp>

namespace spb{

int Vectorization::operator_id = -1;

void Vectorization::op(Item &item){
	if(operator_id < 0){
		operator_id = SPBench::getNewOpId();
	}
	Metrics metrics;
	volatile unsigned long latency_op;
	if(metrics.latency_is_enabled()){
		latency_op = current_time_usecs();
	}
	unsigned int num_item = 0;

	while(num_item < item.batch_size){ //batch loop

		vectorization_op(*item.item_batch[num_item]);

		num_item++;
	}
	
	if(metrics.latency_is_enabled()){
		item.latency_op[operator_id] = (current_time_usecs() - latency_op);
	}
}

}
