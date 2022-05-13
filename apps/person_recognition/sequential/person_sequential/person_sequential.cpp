#include <person_recognition.hpp>

int main (int argc, char* argv[]){
	// Disabling internal OpenCV's support for multithreading 
	cv::setNumThreads(0);
	spb::init_bench(argc, argv); // Initializations
	spb::Metrics::init();
	while(1){
		spb::Item item;
		if(!spb::Source::op(item)) break;
		spb::Detect::op(item); //detect faces in the image:
		spb::Recognize::op(item); //analyze each detected face:
		spb::Sink::op(item);
	}
	spb::Metrics::stop();
	spb::end_bench();
	return 0;
}

