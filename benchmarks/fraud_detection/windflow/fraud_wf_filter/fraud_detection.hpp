/**
 * ************************************************************************  
 *  File  : fraud_detection.hpp
 *
 *  Title : SPBench version of the Fraud Detection
 *
 *  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 *
 *  Date  : December 20, 2022
 *
 * ************************************************************************
**/

#ifndef FRAUD_DETECTION_H
#define FRAUD_DETECTION_H

#include <fraud_detection_utils.hpp>

namespace spb{
class Predictor;

class Predictor{
private:
	static inline void predictor_op(Item &item, Markov_Model_Predictor &predictor);
public:
	static void op(Item &item, Markov_Model_Predictor &predictor);
	Predictor(Item &item, Markov_Model_Predictor &predictor){
		op(item, predictor);
	}
	Predictor(){};

	virtual ~Predictor(){}
};

} // end of namespace spb
#endif