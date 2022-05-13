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

#ifndef FERRET_U
#define FERRET_U

#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include <stack>
#include "include/cass.h"
#include "include/cass_timer.h"
#include "image/image.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <sys/time.h>

#include <spbench.hpp>

#ifdef ENABLE_PARSEC_HOOKS
#include <hooks.h>
#endif

#define DEFAULT_DEPTH   25
#define MAXR            100
#define IMAGE_DIM       14

namespace spb{

class Item;
class Source;
class Sink;

struct load_data;
struct seg_data;
struct extract_data;
struct vec_query_data;
struct rank_data;
struct item_data;

void init_bench(int argc, char* argv[]);
void end_bench();

extern char *extra_params;
extern unsigned int top_K;

extern cass_table_t *table;
extern cass_table_t *query_table;

extern int vec_dist_id;
extern int vecset_dist_id;

struct load_data
{
	int width, height;
	char *name;
	unsigned char *HSV, *RGB;
};

struct seg_data
{
	int width, height, nrgn;
	char *name;
	unsigned char *mask;
	unsigned char *HSV;
};

struct extract_data
{
	cass_dataset_t ds;
	char *name;
};

struct vec_query_data
{
	char *name;
	cass_dataset_t *ds;
	cass_result_t result;
	cass_query_t query;
};

struct rank_data
{
	char *name;
	cass_dataset_t *ds;
	cass_result_t result;
	cass_query_t query;
};

struct item_data {
	union {
		struct load_data      load;
		struct rank_data      rank;
	} first;
	union {
		struct seg_data       seg;
		struct vec_query_data vec;
	} second;
	struct extract_data extract;
	unsigned int index;

	item_data():
		index(0)
	{};

	~item_data(){};
};

class Item{
	public:
		std::vector<item_data*> item_batch;
		unsigned int batch_size;
		unsigned int batch_index;

		std::vector<double> latency_op;
		volatile unsigned long timestamp;

		Item():
			batch_size(0),
			latency_op(6, 0.0),
			timestamp(0.0),
			batch_index(0)
		{};

		~Item(){
			latency_op.clear();
			item_batch.clear();
		}
};

class Source{
public:
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