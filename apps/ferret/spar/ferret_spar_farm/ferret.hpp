/** 
 * ************************************************************************  
 *  File  : ferret.hpp
 *
 *  Title : SPBench version of the Ferret application
 *
 *  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 *
 *  Date  : July 06, 2021
 *
 * ************************************************************************
**/

/** 
 * Copyright (C) 2007 Princeton University
 *       
 * This file is part of Ferret Toolkit.
 * 
 * Ferret Toolkit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
**/


#ifndef FERRET_H
#define FERRET_H

#include <ferret_utils.hpp>

namespace spb{

class Segmentation;
class Extract;
class Vectorization;
class Rank;

class Segmentation{
private:
	static inline void segmentation_op(spb::item_data &item);
	static int operator_id;
public:
	static void op(spb::Item &item);
	Segmentation(spb::Item &item){
		op(item);
	}
    Segmentation(){};
	virtual ~Segmentation(){}
};

class Extract{
private:
	static inline void extract_op(spb::item_data &item);
	static int operator_id;
public:
	static void op(spb::Item &item);
	Extract(spb::Item &item){
		op(item);
	}
    Extract(){};
	virtual ~Extract(){}
};

class Vectorization{
private:
	static inline void vectorization_op(spb::item_data &item);
	static int operator_id;
public:
	static void op(spb::Item &item);
	Vectorization(spb::Item &item){
		op(item);
	}
    Vectorization(){};
	virtual ~Vectorization(){}
};

class Rank{
private:
	static inline void rank_op(spb::item_data &item);
	static int operator_id;
public:
	static void op(spb::Item &item);
	Rank(spb::Item &item){
		op(item);
	}
    Rank(){};
	virtual ~Rank(){}
};

} // end of namespace spb
#endif