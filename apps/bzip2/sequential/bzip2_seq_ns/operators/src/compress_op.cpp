#include <../include/compress_op.hpp>

inline void spb::Compress::compress_op(spb::item_data &item){

    unsigned int outSize = (int) ((item.buffSize*1.01)+600);

    // allocate memory for compressed data
    item.CompDecompData == NULL;
    item.CompDecompData = new char[outSize];

    // make sure memory was allocated properly
    if (item.CompDecompData == NULL)
    {
        fprintf(stderr, "Bzip2: *ERROR: Could not allocate memory (CompressedData)!  Skipping...\n");
        exit(-1);	
    }

    // compress the memory buffer (blocksize=9*100k, verbose=0, worklevel=30)
    int ret = BZ2_bzBuffToBuffCompress(item.CompDecompData, &outSize, item.FileData, item.buffSize, BWTblockSize, Verbosity, 30);

    if (ret != BZ_OK)
        fprintf(stderr, "Bzip2: *ERROR during compression: %d\n", ret);

    item.buffSize = outSize;

}