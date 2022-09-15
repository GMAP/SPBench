#include <person_recognition.hpp>

using namespace spb;
using namespace cv;

Source source;
Detect detect;
Recognize recognize;
Sink sink;

int main (int argc, char* argv[]){
	// Disabling internal OpenCV's support for multithreading.
	setNumThreads(0);

	init_bench(argc, argv); //Initializations
	Metrics::init();

	[[spar::ToStream]]
	while(1){

		Item item;

		if(!source.op(item)) break;

		[[spar::Stage,spar::Input(item),spar::Output(item),spar::Replicate()]]
		{
			detect.op(item); //detect faces in the image:
			recognize.op(item); //analyze each detected face:
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

