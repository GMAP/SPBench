#include <../include/extract_op.hpp>

using namespace spb;

inline void Extract::extract_op(item_data &item){

	item.extract.name = item.second.seg.name;
	image_extract_helper(item.second.seg.HSV,
			item.second.seg.mask,
			item.second.seg.width,
			item.second.seg.height,
			item.second.seg.nrgn,
			&item.extract.ds);
	free(item.second.seg.mask);
	free(item.second.seg.HSV);

}