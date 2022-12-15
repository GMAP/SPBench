#include <../include/segmentation_op.hpp>

inline void spb::Segmentation::segmentation_op(spb::item_data &item){

	item.second.seg.name = item.first.load.name;
	item.second.seg.width = item.first.load.width;
	item.second.seg.height = item.first.load.height;
	item.second.seg.HSV = item.first.load.HSV;
	image_segment((void**)&item.second.seg.mask,
			&item.second.seg.nrgn,
			item.first.load.RGB,
			item.first.load.width,
			item.first.load.height);
	free(item.first.load.RGB);
}