/**
 * ************************************************************************  
 *  File  : fraud_detection_utils.hpp
 *
 *  Title : SPBench version of the Fraud Detection
 *
 *  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 *
 *  Date  : December 20, 2022
 *
 * ************************************************************************
**/

#ifndef FRAUD_DETECTION_U
#define FRAUD_DETECTION_U 

/** Includes: **/
#include <iomanip>
#include "markov_model_prediction/model_based_predictor.hpp"

#include <spbench.hpp>

#define NUMBER_OF_OPERATORS 3

namespace spb{

//extern Markov_Model_Predictor predictor;

struct item_data;
class Item;
class Source;
class Sink;

void init_bench(int argc, char* argv[]);
void end_bench();
/*
struct item_data {

	size_t key;
	std::string record;
	double score;
	unsigned int index;
	bool isOutlier;

	item_data():
		key(0),
		index(0),
		score(0),
		isOutlier(false)
	{};

	~item_data(){}
};
*/
/* This class implements an Item */
class Item : public Batch{
public:
	size_t key;
	std::string record;
	double score;
	unsigned int index;
	bool isOutlier;

	//std::vector<item_data> item_batch;

	Item():
		Batch(NUMBER_OF_OPERATORS),
		key(0),
		index(0),
		score(0),
		isOutlier(false)
	{};

	~Item(){}

	explicit operator bool() const
    { return isOutlier; }

	// Safe aproach to convert the class to kind of bool
	//void some_function () {}
	//typedef void (Item:: * safe_bool_type) ();
  	//operator safe_bool_type () const
  	//{ return isOutlier ? &Item::some_function : 0;}
};

class Source{
public:
	static std::chrono::high_resolution_clock::time_point source_item_timestamp;
	static bool op(Item &item);
	Source(){}
	virtual ~Source(){}
};

class Sink{
public:
	static void op(Item &item);
	Sink(){}
	virtual ~Sink(){}
};

} //end of namespace spb

#endif