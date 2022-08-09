#include <ferret.hpp>

int main(int argc, char *argv[]) {
    /*------------------------------------------------*/
    /* Avoid adding code before the init_bench() call */
    /*------------------------------------------------*/
	spb::init_bench(argc, argv); // Initializations (do not change this line)
	
	// Compact source creation method. This source will run immediately.
	// Parameters: <batch_size>, <batch_interval>, <queue_size>, <frequency>
	spb::Source source1(2, 0, 1, 0);

    // Alternative source creation method
	spb::Source source2;

	// These parameters can be changed anytime during execution
	source2.setBatchSize(1); // 1 item per batch
	source2.setBatchInterval(0.5); // 500 ms batch window
	source2.setQueueMaxSize(3); // 3 slots in this source's queue
	source2.setFrequency(30); // 30 items per second
	
	// You must use the init() method to run this source
	source2.init();

	/* Stream region */
	while(!(source1.depleted() && source2.depleted())){

        spb::Item item1 = source1.getItem();
		spb::Item item2 = source2.getItem();
        
        spb::Segmentation::op(item1);
        spb::Segmentation::op(item2);
        
        spb::Extract::op(item1);
        spb::Extract::op(item2);
        
        spb::Vectorization::op(item1);
        spb::Vectorization::op(item2);
        
        spb::Rank::op(item1);
        spb::Rank::op(item2);
        
        spb::Sink::op(item1);
        spb::Sink::op(item2);
    }

    spb::end_bench(); // Do not change this line
    return 0;
}