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
	static inline void detect_op(item_data &item);
	static int operator_id;
public:
	static void op(Item &item);
	Detect(Item &item){
		op(item);
	}
	Detect(){};

	virtual ~Detect(){}
};

class Recognize{
private:
	static inline void recognize_op(item_data &item);
	static int operator_id;
public:
	static void op(Item &item);
	Recognize(Item &item){
		op(item);
	}
	Recognize(){};
	virtual ~Recognize(){}
};

} // end of namespace spb
#endif