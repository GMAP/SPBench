#include <../include/canny1_op.hpp>

using namespace spb;
using namespace cv;

inline void Canny1::canny1_op(item_data &item){
	//Mat contours;
	Canny(item.imgROI, item.contours,50,250);
	Mat contoursInv;
	threshold(item.contours, contoursInv, 128, 255, THRESH_BINARY_INV);
}
