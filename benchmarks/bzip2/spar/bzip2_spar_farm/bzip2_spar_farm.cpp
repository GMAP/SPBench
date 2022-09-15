#include <bzip2.hpp>

using namespace spb;

Source source;
Compress comp;
Sink sink;

void compress(){
	
	Metrics::init();

	[[spar::ToStream]]
	while (1)
	{
		Item item;
		
		if(!source.op(item)) break;

		[[spar::Stage, spar::Input(item), spar::Output(item), spar::Replicate()]]
		{
			comp.op(item);
		}
		[[spar::Stage,spar::Input(item)]]
		{
			sink.op(item);
		}
	}
	Metrics::stop();
}

Source_d source_d;
Decompress decomp;
Sink_d sink_d;

void decompress(){

	Metrics::init();
	
	[[spar::ToStream]]
	while(1)
	{
		Item item;
		
		if(!source_d.op(item)) break;

		[[spar::Stage, spar::Input(item), spar::Output(item), spar::Replicate()]]
		{
			decomp.op(item);
		}
		[[spar::Stage,spar::Input(item)]]
		{
			sink_d.op(item);
		}
	}
	Metrics::stop();
}

int main (int argc, char* argv[]){
	bzip2_main(argc, argv);
	return 0;
}