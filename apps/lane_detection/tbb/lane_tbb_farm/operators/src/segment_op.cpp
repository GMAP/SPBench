#include <../include/segment_op.hpp>

inline void spb::Segment::segment_op(spb::item_data &item){

	cv::Mat gray;
	if(SPBench::memory_source_is_enabled()){
		cvtColor(*(item.image_p), gray,  CV_RGB2GRAY);
	} else {
		cvtColor(item.image, gray,  CV_RGB2GRAY);
	}
	std::vector<std::string> codes;
	cv::Mat corners;
	findDataMatrix(gray, codes, corners);
	if(SPBench::memory_source_is_enabled()){
		drawDataMatrixCodes(*(item.image_p), codes, corners);
		cv::Rect roi(0, item.image_p->cols/3, item.image_p->cols-1, item.image_p->rows - item.image_p->cols/3);
		cv::Mat aux = *(item.image_p);
		item.imgROI = aux(roi);
	} else {
		drawDataMatrixCodes(item.image, codes, corners);
		cv::Rect roi(0, item.image.cols/3, item.image.cols-1, item.image.rows - item.image.cols/3);
		item.imgROI = item.image(roi);
	}
}

