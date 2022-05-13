#include <bzip2.hpp>

void compress(){

	// Compact source creation method. This source will run immediately.
	// Parameters: <batch_size>, <queue_size>, <reading_frequency>
	spb::Source source1(2, 2, 0);

    // Alternative source creation method
	// This source will not run immediately, you must call the method init() later in order to run it
	spb::Source source2;

	// These parameters can be changed anytime during execution
	source2.setBatchSize(1);	// To set batch size for this source
	source2.setQueueMaxSize(3); // Queue size of this source (each source has its own queue)
	source2.setFrequency(50000);// 50000 = 50 milliseconds
	
	// Calling init() to run this source
	source2.init();

	/* Stream region */
	while (!(source1.depleted() && source2.depleted())){
		spb::Item item1 = source1.getItem();
		spb::Item item2 = source2.getItem();
		
		spb::Compress::op(item1);
		spb::Compress::op(item2);

		spb::Sink::op(item1);
		spb::Sink::op(item2);
	}
	spb::end_bench();
}

void decompress(){

	// Compact source creation method. This source will run immediately.
	// Parameters: <batch_size>, <queue_size>, <reading_frequency>
	spb::Source source1(1, 2, 0);

    // Alternative source creation method
	// This source will not run immediately, you must call the method init() later in order to run it
	spb::Source source2;

	// These parameters can be changed anytime during execution
	source2.setBatchSize(1);	// To set batch size for this source
	source2.setQueueMaxSize(3); // Queue size of this source (each source has its own queue)
	source2.setFrequency(50000);// 50000 = 50 milliseconds
	
	// Calling init() to run this source
	source2.init();

	/* Stream region */
	while(!(source1.depleted() && source2.depleted())){

		spb::Item item1 = source1.getItem();
		spb::Item item2 = source2.getItem();

		spb::Decompress::op(item1);
		spb::Decompress::op(item2);

		spb::Sink::op(item1);
		spb::Sink::op(item2);
	}
	spb::end_bench();
}

int main (int argc, char* argv[]){
	spb::bzip2_main(argc, argv);
	return 0;
}