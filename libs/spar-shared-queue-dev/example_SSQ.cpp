//This is an example on how to use SPar Shared Queue

#include <omp.h>
#include <iostream>

#include "SPar_Shared_Queue.hpp"

#define QUEUESIZE 1024
#define NUMBER_OF_ELEMENTS 1000
#define NTHREADS 8

struct data {
	data(){};
	int number;
	// Must have bool eos field
	bool eos;
};

int main(){

	// Needs 1 queue for each producer consumer relation!

	// arguments SParSharedQueue <data type> ( queue_size,  number_of_producers)
	SParSharedQueue<struct data> * queue1 = new SParSharedQueue<struct data>(QUEUESIZE*NTHREADS,1);
	SParSharedQueue<struct data> * queue2 = new SParSharedQueue<struct data>(QUEUESIZE*NTHREADS,NTHREADS);

	// Data generation stage
	{
		struct data * local;
		for(int i=0;i<NUMBER_OF_ELEMENTS;i++){
			local = new struct data();
			local->number=i;
			// Remember to always set eos to false
			local->eos=false;
			queue1->Add(local);
		}
		queue1->NotifyEOS();
	}

	
	// Parallel processing stage
	{
		struct data * local;
		while(1){
			local = queue1->Remove();
			if(local->eos)
				break;

			// Stage code goes here

			queue2->Add(local);
		}

		queue2->NotifyEOS();
	}

	// Data collection stage
	{
		struct data * local;
		while(1){

			local = queue2->Remove();
			if(local->eos)
				break;

			// Stage code goes here
			
			delete local;
		}
	}

}