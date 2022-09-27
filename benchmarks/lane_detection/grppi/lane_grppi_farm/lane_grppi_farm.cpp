#include <lane_detection.hpp>
#include "grppi/grppi.h"
#include "dyn/dynamic_execution.h"

using namespace std;
using namespace experimental;

void run(grppi::dynamic_execution & ex) {
	tbb::task_scheduler_init init(spb::nthreads);
	grppi::pipeline(ex,
		[]() mutable -> optional<spb::Item> {
			spb::Item item;
			if(!spb::Source::op(item)) {
					return {};
			}else { return item; }
		},
		grppi::farm(spb::nthreads,
			[](spb::Item item) {
			spb::Segment::op(item);
			spb::Canny1::op(item);
			spb::HoughT::op(item);
			spb::HoughP::op(item);
			spb::Bitwise::op(item);
			spb::Canny2::op(item);
			spb::Overlap::op(item);
			return item;
		}),
		[](spb::Item item) {
			spb::Sink::op(item); 
		}
	);
}

grppi::dynamic_execution execution_mode(){
	string backend = spb::SPBench::getArg(0);
	if(backend == "tbb"){
		auto ex = grppi::parallel_execution_tbb(spb::nthreads, true);
		ex.set_queue_attributes(1, grppi::queue_mode::blocking, spb::nthreads*10);
		return ex;
	} else if (backend == "ff"){
		auto ex = grppi::parallel_execution_ff(spb::nthreads, true);
		return ex;
	} else if (backend == "omp"){
		auto ex = grppi::parallel_execution_omp(spb::nthreads, true);
		ex.set_queue_attributes(1, grppi::queue_mode::blocking);
		return ex;
	} else if (backend == "thr"){
		auto ex = grppi::parallel_execution_native(spb::nthreads, true);
		ex.set_queue_attributes(1, grppi::queue_mode::blocking);
		return ex;
	} else {
		cout << "No backend selected" << endl;
		exit(1);
	}
}

int main (int argc, char* argv[]){
	// Disabling internal OpenCV's support for multithreading.
	cv::setNumThreads(0);
	spb::init_bench(argc, argv); //Initializations
	spb::Metrics::init();
	auto ex = execution_mode();
	run(ex);
	spb::Metrics::stop();
	spb::end_bench();
	return 0;
}


