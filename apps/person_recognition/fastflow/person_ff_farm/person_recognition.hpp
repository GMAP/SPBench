/**
 * ************************************************************************  
 *  File  : person_recognition.hpp
 *
 *  Title : SPBench version of the Person Recognition
 *
 *  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 *
 *  Date  : July 06, 2021
 *
 * ************************************************************************
**/

#ifndef PERSON_H
#define PERSON_H

#include <person_recognition_utils.hpp>

namespace spb{

class Detect;
class Recognize;

class Detect{
private:
	static inline void detect_op(spb::item_data &item);
public:
	static void op(spb::Item &item);
	Detect(spb::Item &item){
		op(item);
	}
	virtual ~Detect(){}
};

class Recognize{
private:
	static inline void recognize_op(spb::item_data &item);
public:
	static void op(spb::Item &item);
	Recognize(spb::Item &item){
		op(item);
	}
	virtual ~Recognize(){}
};

} // end of namespace spb
#endif