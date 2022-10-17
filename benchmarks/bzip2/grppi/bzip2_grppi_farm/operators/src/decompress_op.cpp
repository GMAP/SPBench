#include <../include/decompress_op.hpp>

inline void spb::Decompress::decompress_op(spb::item_data &item){

    //int blockNum = 0;
#ifdef PBZIP_DEBUG
    fprintf(stderr, "consumer:  Buffer: %x  Size: %u   Block: %d\n", item.FileData, item.buffSize, blockNum);
#endif

#ifdef PBZIP_DEBUG
    printf ("consumer: recieved %d.\n", blockNum);
#endif

    unsigned int outSize = 900000;
    
    // allocate memory for decompressed data (start with default 900k block size)
    item.CompDecompData = new char[outSize];
    // make sure memory was allocated properly
    if (item.CompDecompData == NULL)
    {
        fprintf(stderr, " *ERROR: Could not allocate memory (DecompressedData)!  Skipping...\n");
        exit(-1);
    }

    // decompress the memory buffer (verbose=0)
    int ret = BZ2_bzBuffToBuffDecompress(item.CompDecompData, &outSize, item.FileData, item.buffSize, 0, Verbosity);

    while (ret == BZ_OUTBUFF_FULL)
    {
#ifdef PBZIP_DEBUG
        fprintf(stderr, "Increasing DecompressedData buffer size: %d -> %d\n", outSize, outSize*4);
#endif

        if (item.CompDecompData != NULL)
            delete [] item.CompDecompData;
        item.CompDecompData = NULL;
        // increase buffer space
        outSize = outSize * 4;
        // allocate memory for decompressed data (start with default 900k block size)
        item.CompDecompData = new char[outSize];
        // make sure memory was allocated properly
        if (item.CompDecompData == NULL)
        {
            fprintf(stderr, "Bzip2: *ERROR: Could not allocate memory (DecompressedData)!  Skipping...\n");
            exit(-1);
        }

        // decompress the memory buffer (verbose=0)
        ret = BZ2_bzBuffToBuffDecompress(item.CompDecompData, &outSize, item.FileData, item.buffSize, 0, Verbosity);

    } // while

    if ((ret != BZ_OK) && (ret != BZ_OUTBUFF_FULL))
        fprintf(stderr, "Bzip2: *ERROR during decompression: %d\n", ret);

#ifdef PBZIP_DEBUG
    fprintf(stderr, "\n Compressed Block Size: %u\n", item.buffSize);
    fprintf(stderr, "   Original Block Size: %u\n", outSize);
#endif

    blockNum++;
    item.buffSize = outSize;

}