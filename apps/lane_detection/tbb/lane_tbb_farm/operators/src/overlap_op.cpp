#include <../include/overlap_op.hpp>

inline void spb::Overlap::overlap_op(spb::item_data &item){

	//set probabilistic Hough parameters
	item.ld.setLineLengthAndGap(5,2);
	item.ld.setMinVote(1);
	if(SPBench::memory_source_is_enabled()){
		item.ld.setShift(item.image_p->cols/3);
		item.ld.drawDetectedLines(*(item.image_p));
	} else {
		item.ld.setShift(item.image.cols/3);
		item.ld.drawDetectedLines(item.image);
	}
	std::stringstream stream;
	stream << "Line Segments: " << item.lines.size();

	if(SPBench::memory_source_is_enabled()) {
		cv::putText(*(item.image_p), stream.str(), cv::Point(10, item.image_p->rows-10), 2, 0.8, cv::Scalar(0,0,255),0);
	} else {
		cv::putText(item.image, stream.str(), cv::Point(10, item.image.rows-10), 2, 0.8, cv::Scalar(0,0,255),0);
	}
	item.lines.clear();
}