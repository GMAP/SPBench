#include <bzip2.hpp>

void compress(){
	spb::Metrics::init_metrics();
	while (1){
		spb::Item item;
		if(!spb::Source::op(item)) break;
		spb::Compress::op(item);
		spb::Sink::op(item);
	}
	spb::Metrics::stop_metrics(metrics);
}

void decompress(){
	spb::Metrics::init_metrics();
	while(1){
		spb::Item item;
		if(!spb::Source_d::op(item)) break;
		spb::Decompress::op(item);
		spb::Sink_d::op(item);
	}
	spb::Metrics::stop_metrics(metrics);
}

int main (int argc, char* argv[]){
	spb::bzip2_main(argc, argv);
	return 0;
}