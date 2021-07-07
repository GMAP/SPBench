#include <metrics.hpp>
#include <ferret.hpp>

int main(int argc, char *argv[]) {
	init_bench(argc, argv);
	data_metrics metrics = init_metrics();

	[[spar::ToStream]] while(1) {
		Item item;
		if(!source_op(item)) break;
		[[spar::Stage,spar::Input(item),spar::Output(item),spar::Replicate()]]
		{
			segmentation_op(item);
		}
		[[spar::Stage,spar::Input(item),spar::Output(item),spar::Replicate()]]
		{
			extract_op(item);
		}
		[[spar::Stage,spar::Input(item),spar::Output(item),spar::Replicate()]]
		{
			vectorization_op(item);
		}
		[[spar::Stage,spar::Input(item),spar::Output(item),spar::Replicate()]]
		{
			rank_op(item);
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

