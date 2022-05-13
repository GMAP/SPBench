#include <lane_detection.hpp>

int main (int argc, char* argv[]){
	/*------------------------------------------------*/
    /* Avoid adding code before the init_bench() call */
    /*------------------------------------------------*/
	spb::init_bench(argc, argv); // Initializations (do not change this line)

	// Disabling internal OpenCV's support for multithreading.
	cv::setNumThreads(0);

	// Compact source creation method. This source will run immediately.
	// Parameters: <batch_size>, <queue_size>, <reading_frequency>
	spb::Source source1(2, 2, 0);

    // Alternative source creation method
	spb::Source source2;

	// These parameters can be changed anytime during execution
	source2.setBatchSize(1);
	source2.setQueueMaxSize(3);
	source2.setFrequency(50000); // 50000 = 50 milliseconds
	
	// You must use the init() method to run this source
	source2.init();

	/* Stream region */
	while(!(source1.depleted() && source2.depleted())){
		
		spb::Item item1 = source1.getItem();
		spb::Item item2 = source2.getItem();

		spb::Segment::op(item1);
		spb::Segment::op(item2);

		spb::Canny1::op(item1);
		spb::Canny1::op(item2);

		spb::HoughT::op(item1);
		spb::HoughT::op(item2);
		
		spb::HoughP::op(item1);
		spb::HoughP::op(item2);

		spb::Bitwise::op(item1);
		spb::Bitwise::op(item2);

		spb::Canny2::op(item1);
		spb::Canny2::op(item2);

		spb::Overlap::op(item1);
		spb::Overlap::op(item2);

		spb::Sink::op(item1);
		spb::Sink::op(item2);
	}

	spb::end_bench(); // Do not change this line
	return 0;
}
