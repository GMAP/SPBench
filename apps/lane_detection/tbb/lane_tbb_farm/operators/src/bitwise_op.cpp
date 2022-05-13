#include <../include/bitwise_op.hpp>

inline void spb::Bitwise::bitwise_op(spb::item_data &item){

	//bitwise AND of the two hough images
	bitwise_and(item.houghP, item.hough, item.houghP);
	cv::Mat houghPinv(item.imgROI.size(), CV_8U, cv::Scalar(0));
	//threshold and invert to black lines
	threshold(item.houghP, houghPinv, 150, 255, cv::THRESH_BINARY_INV);

	item.houghPinv = houghPinv;

}