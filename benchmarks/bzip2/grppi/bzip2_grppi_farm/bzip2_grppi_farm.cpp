#include <bzip2.hpp>
#include "grppi/grppi.h"
#include "dyn/dynamic_execution.h"

using namespace std;
using namespace experimental;

void run_compress(grppi::dynamic_execution & ex) {
	tbb::task_scheduler_init init(spb::nthreads);
	grppi::pipeline(ex,
		[]() mutable -> optional<spb::Item> {
			spb::Item item;
			if(!spb::Source::op(item)) {
					return {};
			}
			return item;
		},
		grppi::farm(spb::nthreads,
			[](spb::Item item) {
				spb::Compress::op(item);                 
				return item;
		}),
		[](spb::Item item) {spb::Sink::op(item); }
	);
}

void run_decompress(grppi::dynamic_execution & ex) {
	tbb::task_scheduler_init init(spb::nthreads);
	grppi::pipeline(ex,
		[]() mutable -> optional<spb::Item> {
			spb::Item item;
			if(!spb::Source_d::op(item)) {
					return {};
			} else {
					return item;
			}
		},
		grppi::farm(spb::nthreads,
			[](spb::Item item) {
				spb::Decompress::op(item);                 
				return item;
		}),
		[](spb::Item item) {spb::Sink_d::op(item); }
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
		cout << " No backend selected. Usage: \'./spbench ... -user-arg <backend>\'. Backends available: tbb, thr, omp, ff." << endl;
		exit(1);
	}
}

void compress(){
	spb::Metrics::init();
	auto ex = execution_mode();
	run_compress(ex);
	spb::Metrics::stop();
}

void decompress(){
	spb::Metrics::init();
	auto ex = execution_mode();
	run_decompress(ex);
	spb::Metrics::stop();
}

int main (int argc, char* argv[]){
	spb::bzip2_main(argc, argv);
	return 0;
}
