#ifndef BZIP2_H
#define BZIP2_H

#include <bzip2_utils.hpp>

namespace spb{

class Compress;
class Decompress;

class Compress{
private:
	static inline void compress_op(spb::item_data &item);
	static int operator_id;
public:
	static void op(spb::Item &item);
	Compress(spb::Item &item){
		op(item);
	}
    Compress(){};

	virtual ~Compress(){}
};

class Decompress{
private:
	static inline void decompress_op(spb::item_data &item);
	static int operator_id;
public:
	static void op(spb::Item &item);
	Decompress(spb::Item &item){
		op(item);
	}
    Decompress(){};
	virtual ~Decompress(){}
};

} // end of namespace spb
#endif
