#include <../include/bitwise_op.hpp>

using namespace spb;
using namespace cv;

inline void Bitwise::bitwise_op(item_data &item){

	//bitwise AND of the two hough images
	bitwise_and(item.houghP, item.hough, item.houghP);
	Mat houghPinv(item.imgROI.size(), CV_8U, Scalar(0));
	//threshold and invert to black lines
	threshold(item.houghP, houghPinv, 150, 255, THRESH_BINARY_INV);

	item.houghPinv = houghPinv;

}