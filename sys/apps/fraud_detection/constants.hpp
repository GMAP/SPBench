/** 
 *  @file    constants.hpp
 *  @author  Alessandra Fais
 *  @date    11/05/2019
 *  
 *  @brief Definition of useful constants
 */ 

#ifndef FRAUDDETECTION_CONSTANTS_HPP
#define FRAUDDETECTION_CONSTANTS_HPP

#include <stdlib.h>

size_t _records_win_size = 5;
size_t _state_position = 1;
detection_algorithm _alg = MISS_PROBABILITY;
double _threshold = 0.96;

const std::string _model_file = "/model/model.txt";

#endif //FRAUDDETECTION_CONSTANTS_HPP
