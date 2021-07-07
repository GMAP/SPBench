#include <metrics.hpp>
#include <bzip2.hpp>

using namespace std;

void compress(){
	data_metrics metrics = init_metrics();
	while (bytesLeft > 0){
		Item item;
		if(!read_comp_op(item)) break;
		compress_op(item);
		write_comp_op(item);
	}
	stop_metrics(metrics);
}

void decompress(){
	data_metrics metrics = init_metrics();
	while(item_count < bz2NumBlocks){
		Item item;
		if(!read_decomp_op(item)) break;
		decompress_op(item);
		write_decomp_op(item);
	}
	stop_metrics(metrics);
}

int main (int argc, char* argv[]){
	bzip2_main(argc, argv);
	return 0;
}
