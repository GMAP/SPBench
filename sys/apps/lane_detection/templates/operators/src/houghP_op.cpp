#include <../include/houghP_op.hpp>

inline void spb::HoughP::houghP_op(spb::item_data &item){

	//set probabilistic Hough parameters
	item.ld.setLineLengthAndGap(60,10);
	item.ld.setMinVote(4);

	//detect lines
	item.li = item.ld.findLines(item.contours);
	cv::Mat houghP(item.imgROI.size(), CV_8U, cv::Scalar(0));
	item.ld.setShift(0);
	item.ld.drawDetectedLines(houghP);

	item.houghP = houghP;
}