#include <../include/canny1_op.hpp>

inline void spb::Canny1::canny1_op(spb::item_data &item){
	//Mat contours;
	cv::Canny(item.imgROI, item.contours,50,250);
	cv::Mat contoursInv;
	cv::threshold(item.contours, contoursInv, 128, 255, cv::THRESH_BINARY_INV);
}
