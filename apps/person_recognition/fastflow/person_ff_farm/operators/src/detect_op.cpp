#include <../include/detect_op.hpp>

inline void spb::Detect::detect_op(spb::item_data &item){

	cv::CascadeClassifier _cascade;
	_cascade.load(std::string(input_data.cascade_path));

	cv::Mat tmp;
	int width, height;
	if(SPBench::memory_source_is_enabled()){
		width  = item.image_p->size().width;
		height = item.image_p->size().height;
	} else {
		width  = item.image.size().width;
		height = item.image.size().height;
	}

	cv::Size minScaleSize = cv::Size(DET_MIN_SIZE_RATIO  * width, DET_MIN_SIZE_RATIO  * height),
		maxScaleSize = cv::Size(DET_MAX_SIZE_RATIO  * width, DET_MAX_SIZE_RATIO  * height);

	if(SPBench::memory_source_is_enabled()){
		cvtColor(*(item.image_p), tmp, CV_BGR2GRAY);
	} else {
		cvtColor(item.image, tmp, CV_BGR2GRAY);
	}
	//convert the image to grayscale and normalize histogram:
	equalizeHist(tmp, tmp);

	//clear the vector:
	item.faces.clear();

	//detect faces:
	_cascade.detectMultiScale(tmp, item.faces, DET_SCALE_FACTOR, DET_MIN_NEIGHBORS, 0, minScaleSize, maxScaleSize);

}