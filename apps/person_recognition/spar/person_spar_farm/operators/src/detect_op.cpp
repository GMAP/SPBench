#include <../include/detect_op.hpp>

using namespace spb;
using namespace std;
using namespace cv;

inline void Detect::detect_op(item_data &item){

	CascadeClassifier _cascade;
	_cascade.load(string(input_data.cascade_path));

	SPBench spbench;

	Mat tmp;
	int width, height;
	if(spbench.memory_source_is_enabled()){
		width  = item.image_p->size().width;
		height = item.image_p->size().height;
	} else {
		width  = item.image.size().width;
		height = item.image.size().height;
	}

	Size minScaleSize = Size(DET_MIN_SIZE_RATIO  * width, DET_MIN_SIZE_RATIO  * height),
		maxScaleSize = Size(DET_MAX_SIZE_RATIO  * width, DET_MAX_SIZE_RATIO  * height);

	if(spbench.memory_source_is_enabled()){
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