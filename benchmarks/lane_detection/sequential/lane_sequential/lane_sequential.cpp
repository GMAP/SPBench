#include <lane_detection.hpp>

std::atomic<bool> done(false);

void stream_region(){
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
	done = true;
	return;
}

int main (int argc, char* argv[]){
	// Disabling internal OpenCV's support for multithreading 
	cv::setNumThreads(0);
	spb::init_bench(argc, argv); // Initializations

	spb::Metrics::init();

	std::thread stream_thread(stream_region);

	while(!done){
		sleep(1);
		std::cout << spb::Metrics::getAverageLatency() 
		<< " " << spb::Metrics::getInstantThroughput(2)
		<< std::endl;
	}

	if (stream_thread.joinable()){
		stream_thread.join();
	}
	
	

	spb::Metrics::stop();
	spb::end_bench();
	return 0;
}