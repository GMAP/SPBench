#include <metrics.hpp>
#include <bzip2.hpp>

void compress(){

	data_metrics metrics = init_metrics(); //UPL and throughput

	[[spar::ToStream]]
	while (bytesLeft > 0)
	{
		Item item;
		
		if(!read_comp_op(item)) break;

		[[spar::Stage, spar::Input(item), spar::Output(item), spar::Replicate()]]
		{
			compress_op(item);
		}
		[[spar::Stage,spar::Input(item)]]
		{
			write_comp_op(item);
		}
	}
	stop_metrics(metrics);
}

void decompress(){

	data_metrics metrics = init_metrics();//UPL and throughput
	
	[[spar::ToStream]]
	while(item_count < bz2NumBlocks)
	{
		Item item;
		
		if(!read_decomp_op(item)) break;

		[[spar::Stage, spar::Input(item), spar::Output(item), spar::Replicate()]]
		{
			decompress_op(item);
		}
		[[spar::Stage,spar::Input(item)]]
		{
			write_decomp_op(item);
		}
	}
	stop_metrics(metrics);
}

int main (int argc, char* argv[]){
	bzip2_main(argc, argv);
	return 0;
}