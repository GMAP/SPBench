#include <lane_detection.hpp>

using namespace spb;
using namespace cv;

Source source;
Segment segment;
Canny1 canny1;
HoughT houghT;
HoughP houghP;
Bitwise bitwise;
Canny2 canny2;
Overlap overlap;
Sink sink;

int main (int argc, char* argv[]){

	// Disabling internal OpenCV's support for multithreading.
	setNumThreads(0);

	init_bench(argc, argv); //Initializations

	Metrics::init();
	[[spar::ToStream]]
	while(1){
		Item item;
		if (!source.op(item)) break;
		[[spar::Stage,spar::Input(item),spar::Output(item),spar::Replicate()]]
		{
			segment.op(item);
			canny1.op(item);
			houghT.op(item);
			houghP.op(item);
			bitwise.op(item);
			canny2.op(item);
			overlap.op(item);
		}
		[[spar::Stage,spar::Input(item)]]
		{
			sink.op(item);
		}
	}

	Metrics::stop();
	end_bench();
	return 0;
}
