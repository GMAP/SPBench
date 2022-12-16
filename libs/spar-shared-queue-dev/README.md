# spar-shared-queue-dev

This is a shared memory MPMC (Multiple Consumers Multiple Producers) queue.
Implemented using C++ standard mutex and conditional variables.

No busy-waiting.
Mutually exclusive access.
FIFO access.

How to use:
	Example found in example_SSQ.cpp

	-> This version works with pointer type elements in the queue
	-> Construct Queue with SParSharedQueue <data type> ( queue_size,  number_of_producers)
	-> One queue for each producer - consumer relation
	-> Remember to add bool eos to your data type struct
	-> Producers add in queue with: Add(in_var)
	-> Consumers remove from queue with: out_var = Remove()
	-> After removing, always verify if its the end of stream with if(out_var.eos)
	-> Producers must inform the end of stream with NotifyEOS()
