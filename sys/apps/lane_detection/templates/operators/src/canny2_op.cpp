#include <../include/canny2_op.hpp>

inline void spb::Canny2::canny2_op(spb::item_data &item){

	cv::Canny(item.houghPinv, item.contours, 100, 350);
	item.li = item.ld.findLines(item.contours);
}