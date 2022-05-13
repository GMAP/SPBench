#include <../include/segment_op.hpp>

using namespace spb;
using namespace cv;
using namespace std;


inline void Segment::segment_op(item_data &item){

	Mat gray;
	if(SPBench::memory_source_is_enabled()){
		cvtColor(*(item.image_p), gray,  CV_RGB2GRAY);
	} else {
		cvtColor(item.image, gray,  CV_RGB2GRAY);
	}
	vector<string> codes;
	Mat corners;
	findDataMatrix(gray, codes, corners);
	if(SPBench::memory_source_is_enabled()){
		drawDataMatrixCodes(*(item.image_p), codes, corners);
		Rect roi(0, item.image_p->cols/3, item.image_p->cols-1, item.image_p->rows - item.image_p->cols/3);
		Mat aux = *(item.image_p);
		item.imgROI = aux(roi);
	} else {
		drawDataMatrixCodes(item.image, codes, corners);
		Rect roi(0, item.image.cols/3, item.image.cols-1, item.image.rows - item.image.cols/3);
		item.imgROI = item.image(roi);
	}
}

