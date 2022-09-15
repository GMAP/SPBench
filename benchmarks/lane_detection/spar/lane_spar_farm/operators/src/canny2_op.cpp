#include <../include/canny2_op.hpp>

using namespace spb;
using namespace cv;

inline void Canny2::canny2_op(item_data &item){

	Canny(item.houghPinv, item.contours, 100, 350);
	item.li = item.ld.findLines(item.contours);
}