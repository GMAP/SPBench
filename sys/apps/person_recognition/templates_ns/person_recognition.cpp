#include <person_recognition.hpp>

int main (int argc, char* argv[]){
	/*------------------------------------------------*/
    /* Avoid adding code before the init_bench() call */
    /*------------------------------------------------*/
	spb::init_bench(argc, argv); // Initializations (do not change this line)

	cv::setNumThreads(0); // Disabling internal OpenCV's support for multithreading
	
	// Compact source creation method. This source will run immediately.
	// Parameters: <batch_size>, <queue_size>, <reading_frequency>
	spb::Source source1(2, 2, 0);

    // Alternative source creation method
	spb::Source source2;

	// These parameters can be changed anytime during execution
	source2.setBatchSize(1);
	source2.setQueueMaxSize(3);
	source2.setFrequency(50000); // 50000 = 50 milliseconds
	
	// You must use init() to run this source
	source2.init();

	/* Stream region */
	while(!(source1.depleted() && source2.depleted())){

		// Get new item
		spb::Item item1 = source1.getItem();
		spb::Item item2 = source2.getItem();

		// Look for faces in the video frame
		spb::Detect::op(item1);
		spb::Detect::op(item2);

		// Try to recognize the detected faces
		spb::Recognize::op(item1);
		spb::Recognize::op(item2); 
		
		// Write the output video
		spb::Sink::op(item1);
		spb::Sink::op(item2);
	}
	spb::end_bench(); // Do not change this line
	return 0;
}