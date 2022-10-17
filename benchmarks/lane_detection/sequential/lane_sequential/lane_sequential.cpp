#include <lane_detection.hpp>

int main (int argc, char* argv[]){
	// Disabling internal OpenCV's support for multithreading 
	cv::setNumThreads(0);
	spb::init_bench(argc, argv); // Initializations

	spb::Metrics::init();
	while(1){
		spb::Item item;
		if (!spb::Source::op(item)) break;
		spb::Segment::op(item);
		spb::Canny1::op(item);
		spb::HoughT::op(item);
		spb::HoughP::op(item);
		spb::Bitwise::op(item);
		spb::Canny2::op(item);
		spb::Overlap::op(item);
		spb::Sink::op(item);
	}
	spb::Metrics::stop();
	spb::end_bench();
	return 0;
}