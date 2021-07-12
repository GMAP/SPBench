#include <metrics.hpp>
#include <lane_detection.hpp>

int main (int argc, char* argv[]){

	// Disabling internal OpenCV's support for multithreading.
	setNumThreads(0);

	init_bench(argc, argv); //Initializations

	data_metrics metrics = init_metrics(); //UPL and throughput

	[[spar::ToStream]]
	while(1){

		Item item;

		if (!source_op(item)) break;

		[[spar::Stage,spar::Input(item),spar::Output(item),spar::Replicate()]]
		{
			segment_op(item);
			canny1_op(item);
			houghT_op(item);
			houghP_op(item);
			bitwise_op(item);
			canny2_op(item);
			overlap_op(item);
		}
		[[spar::Stage,spar::Input(item)]]
		{
			sink_op(item);
		}
	}

	stop_metrics(metrics);

	end_bench();
	return 0;
}