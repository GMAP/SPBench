#include <../include/houghP_op.hpp>

using namespace spb;
using namespace cv;

inline void HoughP::houghP_op(item_data &item){

	//set probabilistic Hough parameters
	item.ld.setLineLengthAndGap(60,10);
	item.ld.setMinVote(4);

	//detect lines
	item.li = item.ld.findLines(item.contours);
	Mat houghP(item.imgROI.size(), CV_8U, Scalar(0));
	item.ld.setShift(0);
	item.ld.drawDetectedLines(houghP);

	item.houghP = houghP;
}