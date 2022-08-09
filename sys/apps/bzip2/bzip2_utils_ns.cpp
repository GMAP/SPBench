/** 
 * ************************************************************************  
 *  File  : bzip2_utils_ns.cpp
 *
 *  Title : SPBench version of the parallel BZIP2
 *
 *  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 *
 *  Date  : July 06, 2021
 *
 * ************************************************************************
**/

/*
 *	Original file  : pbzip2.cpp
 *
 *	Title : Parallel BZIP2 (pbzip2)
 *
 *	Author: Jeff Gilchrist (http://gilchrist.ca/jeff/)
 *           - Modified producer/consumer threading code from
 *             Andrae Muys <andrae@humbug.org.au.au>
 *           - uses libbzip2 by Julian Seward (http://sources.redhat.com/bzip2/)
 *
 *	Date  : January 8, 2009
 *
 *
 *  Contributions
 *  -------------
 *  Bryan Stillwell <bryan@bokeoa.com> - code cleanup, RPM spec, prep work
 *			for inclusion in Fedora Extras
 *  Dru Lemley [http://lemley.net/smp.html] - help with large file support
 *  Kir Kolyshkin <kir@sacred.ru> - autodetection for # of CPUs
 *  Joergen Ramskov <joergen@ramskov.org> - initial version of man page
 *  Peter Cordes <peter@cordes.ca> - code cleanup
 *  Kurt Fitzner <kfitzner@excelcia.org> - port to Windows compilers and
 *          decompression throttling
 *  Oliver Falk <oliver@linux-kernel.at> - RPM spec update
 *  Jindrich Novy <jnovy@redhat.com> - code cleanup and bug fixes
 *  Benjamin Reed <ranger@befunk.com> - autodetection for # of CPUs in OSX
 *  Chris Dearman <chris@mips.com> - fixed pthreads race condition
 *  Richard Russon <ntfs@flatcap.org> - help fix decompression bug
 *  Paul Pluzhnikov <paul@parasoft.com> - fixed minor memory leak
 *  Aníbal Monsalve Salazar <anibal@debian.org> - creates and maintains Debian packages
 *  Steve Christensen - creates and maintains Solaris packages (sunfreeware.com)
 *  Alessio Cervellin - creates and maintains Solaris packages (blastwave.org)
 *  Ying-Chieh Liao - created the FreeBSD port
 *  Andrew Pantyukhin <sat@FreeBSD.org> - maintains the FreeBSD ports and willing to
 *          resolve any FreeBSD-related problems
 *  Roland Illig <rillig@NetBSD.org> - creates and maintains NetBSD packages
 *  Matt Turner <mattst88@gmail.com> - code cleanup
 *  Álvaro Reguly <alvaro@reguly.com> - RPM spec update to support SUSE Linux
 *  Ivan Voras <ivoras@freebsd.org> - support for stdin and pipes during compression and
 *          CPU detect changes
 *  John Dalton <john@johndalton.info> - code cleanup and bug fixes for stdin support
 *  Rene Georgi <rene.georgi@online.de> - code and Makefile cleanup, support for direct
 *          decompress and bzcat
 *  René Rhéaume & Jeroen Roovers <jer@xs4all.nl> - patch to support uclibc's lack of
 *          a getloadavg function
 *  Reinhard Schiedermeier <rs@cs.hm.edu> - support for tar --use-compress-prog=pbzip2
 *
 *  Specials thanks for suggestions and testing:  Phillippe Welsh,
 *  James Terhune, Dru Lemley, Bryan Stillwell, George Chalissery,
 *  Kir Kolyshkin, Madhu Kangara, Mike Furr, Joergen Ramskov, Kurt Fitzner,
 *  Peter Cordes, Oliver Falk, Jindrich Novy, Benjamin Reed, Chris Dearman,
 *  Richard Russon, Aníbal Monsalve Salazar, Jim Leonard, Paul Pluzhnikov,
 *  Coran Fisher, Ken Takusagawa, David Pyke, Matt Turner, Damien Ancelin,
 *  Álvaro Reguly, Ivan Voras, John Dalton, Sami Liedes, Rene Georgi, 
 *  René Rhéaume, Jeroen Roovers, Reinhard Schiedermeier, Kari Pahula,
 *  Elbert Pol.
 *
 *
 * This program, "pbzip2" is copyright (C) 2003-2009 Jeff Gilchrist.
 * All rights reserved.
 *
 * The library "libbzip2" which pbzip2 uses, is copyright
 * (C) 1996-2008 Julian R Seward.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
* 2. The origin of this software must not be misrepresented; you must
*    not claim that you wrote the original software.  If you use this
*    software in a product, an acknowledgment in the product
*    documentation would be appreciated but is not required.
*
* 3. Altered source versions must be plainly marked as such, and must
*    not be misrepresented as being the original software.
*
* 4. The name of the author may not be used to endorse or promote
*    products derived from this software without specific prior written
*    permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Jeff Gilchrist, Ottawa, Canada.
* pbzip2@compression.ca
* pbzip2 version 1.0.5 of January 8, 2009
*
*/

#include <bzip2_utils_ns.hpp>

namespace spb{
//
// GLOBALS
//
static int numCPU = 2;
static int NumBlocks = 0;
static int NumBufferedBlocks = 0;
int Verbosity = 0;
static int QuietMode = 1;
//static int OutputStdOut = 0;
static int ForceOverwrite = 0;
int BWTblockSize = 9;
static int FileListCount = 0;
static struct stat fileMetaData;
static char *sigInFilename = NULL;
static char *sigOutFilename = NULL;
static char BWTblockSizeChar = '9';

int blockNum = 0;

std::vector<IO_data_struct> IO_data_vec;

void mySignalCatcher(int);
char *memstr(char *, int, char *, int);

int getFileMetaData(char *);
int writeFileMetaData(char *);
int testBZ2ErrorHandling(int, BZFILE *, int);
int testCompressedData(char *);
ssize_t bufread(int hf, char *buf, size_t bsize);
int detectCPUs(void);
/*
 *********************************************************
 */


//std::vector<int> hOutfile_vec;
int blockSize = 9*100000;


//std::vector <bz2BlockListing> bz2BlockList;

unsigned int global_decomp = 0; //0 for compress and 1 for decompress

//bool optimized_memory = false; 

//std::vector<std::vector<input_data_inmemory>> MemData;


void set_operators_name(){
	if(Metrics::latency_is_enabled()){
		if(global_decomp == 1){
			SPBench::addOperatorName("Read      ");
			SPBench::addOperatorName("Decompress");
			SPBench::addOperatorName("Write     ");
		} else {
			SPBench::addOperatorName("Read    ");
			SPBench::addOperatorName("Compress");
			SPBench::addOperatorName("Write   ");
		}
	}
}

void Source::printStatus(){
	std::cout << "\n - New source added: " << getSourceName() << std::endl;

	std::cout << "\n - Workload:" << std::endl;
	std::cout << "         Input file: " << IO_data_vec[sourceId].InFilename << std::endl;
	std::cout << "        Output file: " << IO_data_vec[sourceId].OutFilename << std::endl;

	std::cout << "\n - Setup:" << std::endl;
	std::cout << "         Batch size: " << sourceBatchSize << std::endl;
	std::cout << "     Batch interval: " << sourceBatchInterval << std::endl;
	
	if(sourceQueueSize == 0)
		std::cout << "    Queue max. size: 0 (unlimited)" << std::endl;
	else
		std::cout << "    Queue max. size: " << sourceQueueSize << std::endl;

	if(sourceFrequency == 0)
		std::cout << "    Input frequency: 0 (no limit)" << std::endl;
	else
		std::cout << "    Input frequency: " << sourceFrequency << " items per second" << std::endl;

	if(SPBench::memory_source_is_enabled())
		std::cout << "In-memory execution: enabled" << std::endl;
	std::cout << "\n###############################################" << std::endl;
}

void end_bench(){
	for (auto & input : IO_data_vec){
		close(input.hInfile);
		if(SPBench::memory_source_is_enabled()){
			while(!input.MemData.empty()){
				int ret = write(input.hOutfile, input.MemData[0].block, input.MemData[0].buffSize);
				delete [] input.MemData[0].block;
				input.MemData.erase(input.MemData.begin());
			}
		}
		close(input.hOutfile);
	}
	compute_metrics();
}

bool Source::source_comp(){

	IO_data_vec[sourceId].hOutfile = 1;  // default to stdout

	//CompressedSize = 0;
	
	//currBlock = 0;
	OFF_T bytesLeft = IO_data_vec[sourceId].fileSize;

	if(SPBench::memory_source_is_enabled()){
		OFF_T buffSize;
		input_data_inmemory file_data;

		while (bytesLeft > 0)
		{
			// set buffer size
			if (bytesLeft > blockSize)
				buffSize = blockSize;
			else
				buffSize = bytesLeft;

			file_data.block = new char[buffSize];
			// make sure memory was allocated properly
			if (file_data.block == NULL)
			{
				fprintf(stderr, "Bzip2: *ERROR: Could not allocate memory (FileData)!  Skipping...\n");
				exit(-1);
			}	
			// read file data from disk
			int rret = read(IO_data_vec[sourceId].hInfile, file_data.block, buffSize);	
			// check to make sure all the data we expected was read in
			if (rret != buffSize)
				buffSize = rret;
			//end of the file
			if (rret == 0) 
				break;
			else if (rret < 0){
				fprintf(stderr, "Bzip2: *ERROR: Could not read from file!  Skipping...\n");
				exit(-1);
			}
			IO_data_vec[sourceId].MemData.push_back(file_data);
			bytesLeft -= rret;
		}
		// redefine bytes left
		bytesLeft = IO_data_vec[sourceId].fileSize;
		//close(IO_data_vec[sourceId].hInfile);
	}

	// write to file instead of stdout
	if (IO_data_vec[sourceId].OutputStdOut == 0)
	{
		IO_data_vec[sourceId].hOutfile = open(IO_data_vec[sourceId].OutFilename, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, FILE_MODE);
		// check to see if file creation was successful
		if (IO_data_vec[sourceId].hOutfile == -1)
		{
			fprintf(stderr, "Bzip2: *ERROR: Could not create output file [%s]!\n", IO_data_vec[sourceId].OutFilename);
			exit(-1);
		}
	}

	bool end_of_input = false;
	unsigned int sourceItemCounter = 0;
	long source_item_timestamp = current_time_usecs();


	while(1){

		Item item;

		//if last batch included the last item, ends computation
		if(end_of_input == true){
			item.setLastItem();
			sourceQueue.enqueue(item);
			break;
		}

		// frequency control mechanism
		item_frequency_control(source_item_timestamp, sourceFrequency);

		item.timestamp = source_item_timestamp = current_time_usecs();
		unsigned long batch_elapsed_time = source_item_timestamp;

		volatile unsigned long latency_op;
		item.timestamp = current_time_usecs();
		if(Metrics::latency_is_enabled()){
			latency_op = current_time_usecs();
		}

		while(1){ //batch loop

			// batching management routines
			if(sourceBatchInterval){
				if(((current_time_usecs() - batch_elapsed_time) / 1000.0) >= sourceBatchInterval) break;
			} else {
				if(item.batch_size >= sourceBatchSize) break;
			}
			if(sourceBatchSize > 1){
				if(item.batch_size >= sourceBatchSize) break;
			}

			struct item_data item_data;
			item_data.sourceId = getSourceId();
			// set buffer size
			if (bytesLeft > blockSize)
				item_data.buffSize = blockSize;
			else
				item_data.buffSize = bytesLeft;

			// allocate memory to read in file
			item_data.FileData = NULL;

			if(SPBench::memory_source_is_enabled()){
				//if(optimized_memory){
				item_data.FileData = IO_data_vec[sourceId].MemData[sourceItemCounter].block;
				item_data.index = sourceItemCounter;
				//} else {
				//	item_data.FileData = MemReadData + (IO_data_vec[sourceId].fileSize - bytesLeft);
				//}
				//end of the input
				if (bytesLeft == 0){
					end_of_input = true;
					break;
				} else if (bytesLeft < 0){
					fprintf(stderr, "Bzip2: *ERROR: Could not read from file!  Skipping...\n");
					exit(-1);
				}
				bytesLeft -= item_data.buffSize;
			} else {
				item_data.FileData = new char[item_data.buffSize];

				// read file data from disk
				int ret = read(IO_data_vec[sourceId].hInfile, (char *) item_data.FileData, item_data.buffSize);	

				// check to make sure all the data we expected was read in
				if (ret != item_data.buffSize)
					item_data.buffSize = ret;

				//end of the file
				if (ret == 0){
					end_of_input = true;
					break;
				} else if (ret < 0){
					fprintf(stderr, "Bzip2: *ERROR: Could not read from file!  Skipping...\n");
					exit(-1);
				}
				bytesLeft -= ret;
			}
			item.item_batch.resize(item.batch_size+1);
			//item_data.index = sourceItemCounter;
			item.item_batch[item.batch_size] = item_data;
			item.batch_size++;
			item.setNotEmpty();
			sourceItemCounter++;
		}

		//if this batch has size 0, ends computation
		if(item.batch_size == 0){
			item.setLastItem();
			sourceQueue.enqueue(item);
			break;
		}

		if(Metrics::latency_is_enabled()){
			item.latency_op[0] = (current_time_usecs() - latency_op);
		}

		// put item in the output queue
		sourceQueue.enqueue(item);

		// Accumulate the total number of sent batches
		metrics_vec[sourceId].global_batch_counter++;

		// Update the total number of sent items
		metrics_vec[sourceId].global_item_counter = sourceItemCounter;
	}
	
	return false;
}

void Sink::sink_c(Item &item){

	if(item.empty())
		return;

	volatile unsigned long latency_op;
	if(Metrics::latency_is_enabled()){
		latency_op = current_time_usecs();
	}

	unsigned int num_item = 0;
	while(num_item < item.batch_size){ //batch loop
		struct item_data item_data;
		item_data = item.item_batch[num_item];
		
		if(SPBench::memory_source_is_enabled()){
			delete [] IO_data_vec[item.sourceId].MemData[item_data.index].block;
			IO_data_vec[item.sourceId].MemData[item_data.index].block = item_data.CompDecompData;
			IO_data_vec[item.sourceId].MemData[item_data.index].buffSize = item_data.buffSize;
		} else {
			int ret = write(IO_data_vec[item.sourceId].hOutfile, item_data.CompDecompData, item_data.buffSize);
			if (ret <= 0)
			{
				fprintf(stderr, "Bzip2: *ERROR: Could not write to file! Skipping...\n");
				exit(-1);
			}
		}

		if(!SPBench::memory_source_is_enabled()){
			if (item_data.FileData != NULL)
				delete [] item_data.FileData;
		}

		/*if(!optimized_memory){
			if (item_data.CompDecompData != NULL)
				delete [] item_data.CompDecompData;
		}*/
		num_item++;
	}
	
	metrics_vec[item.sourceId].batches_at_sink_counter++;

	if(Metrics::monitoring_is_enabled()){
		monitor_metrics(item.timestamp, item.sourceId);
	}
	if(Metrics::latency_is_enabled()){

		double current_time_sink = current_time_usecs();
		item.latency_op[2] = (current_time_sink - latency_op);

		volatile unsigned long total_item_latency = (current_time_sink - item.timestamp);
		metrics_vec[item.sourceId].global_latency_acc += total_item_latency; // to compute real time average latency
		
		item_metrics_data latency;
		latency.local_latency = item.latency_op;
		latency.total_latency = total_item_latency;
		latency.item_sink_timestamp = current_time_sink;
		latency.batch_size = item.batch_size;
		metrics_vec[item.sourceId].latency_vector_ns.push_back(latency);
		metrics_vec[item.sourceId].stop_throughput_clock = current_time_sink;
		item.latency_op.clear();
	}
}

int direct_compress()
{
	compress();
	return 0;
}

/*
 *********************************************************
 */


void Source::source_decomp(){

	char bz2Header[] = {"BZh91AY&SY"};  // for 900k BWT block size
	blockNum = 0;

	// set search header to value in file
	bz2Header[3] = BWTblockSizeChar;

	if (IO_data_vec[sourceId].OutputStdOut == 0)
	{
		IO_data_vec[sourceId].hOutfile = open(IO_data_vec[sourceId].OutFilename, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, FILE_MODE);
		// check to see if file creation was successful
		if (IO_data_vec[sourceId].hOutfile == -1)
		{
			fprintf(stderr, "Bzip2: *ERROR: Could not create output file [%s]!\n", IO_data_vec[sourceId].OutFilename);
			exit(-1);
		}
	}	

	// go to start of file
	int ret = lseek(IO_data_vec[sourceId].hInfile, 0, SEEK_SET);
	if (ret != 0)
	{
		fprintf(stderr, "Bzip2: *ERROR: Could not seek to beginning of file [%" "llu" "]!  Skipping...\n", (unsigned long long)ret);
		close(IO_data_vec[sourceId].hInfile);
		exit(-1);
	}

	char *FileData;
	OFF_T inSize = 100000;

	// scan input file for BZIP2 block markers (BZh91AY&SY)
	// allocate memory to read in file
	FileData = NULL;
	FileData = new char[inSize];
	// make sure memory was allocated properly
	if (FileData == NULL)
	{
		fprintf(stderr, "Bzip2: *ERROR: Could not allocate memory (FileData)!  Skipping...\n");
		close(IO_data_vec[sourceId].hInfile);
		exit(-1);
	}

	//Creating variables.
	OFF_T bytesLeft = IO_data_vec[sourceId].fileSize;
	OFF_T currentByte = 0;
	OFF_T startByte = 0;

	int bz2NumBlocks = 0;

	bz2BlockListing TempBlockListing;
	char *startPointer = NULL;

	// keep going until all the file is scanned for BZIP2 blocks
	while (bytesLeft > 0)
	{
		if (currentByte == 0)
		{
#ifdef PBZIP_DEBUG
			fprintf(stderr, " -> Bytes To Read: %" "llu" " bytes...\n", inSize);
#endif
			// read file data
			ret = read(IO_data_vec[sourceId].hInfile, (char *) FileData, inSize);
		}
		else
		{
			// copy end section of previous buffer to new just in case the BZIP2 header is
			// located between two buffer boundaries
			memcpy(FileData, FileData+inSize-(strlen(bz2Header)-1), strlen(bz2Header)-1);
#ifdef PBZIP_DEBUG
			fprintf(stderr, " -> Bytes To Read: %" "llu" " bytes...\n", inSize-(strlen(bz2Header)-1));
#endif
			// read file data minus overflow from previous buffer
			ret = read(IO_data_vec[sourceId].hInfile, (char *) FileData+strlen(bz2Header)-1, inSize-(strlen(bz2Header)-1));
		}
#ifdef PBZIP_DEBUG
		fprintf(stderr, " -> Total Bytes Read: %" "llu" " bytes...\n\n", ret);
#endif
		if (ret < 0)
		{
			fprintf(stderr, "Bzip2: *ERROR: Could not read from fibz2NumBlocksle!  Skipping...\n");
			close(IO_data_vec[sourceId].hInfile);
			if (FileData != NULL)
				delete [] FileData;
			exit(-1);
		}

		// scan buffer for bzip2 start header
		if (currentByte == 0)
			startPointer = memstr(FileData, ret, bz2Header, strlen(bz2Header));
		else
			startPointer = memstr(FileData, ret+(strlen(bz2Header)-1), bz2Header, strlen(bz2Header));
		while (startPointer != NULL)
		{
			if (currentByte == 0)
				startByte = startPointer - FileData + currentByte;
			else
				startByte = startPointer - FileData + currentByte - (strlen(bz2Header) - 1);
#ifdef PBZIP_DEBUG
			fprintf(stderr, " Found substring at: %x\n", startPointer);
			fprintf(stderr, " startByte = %" "llu" "\n", startByte);
			fprintf(stderr, " bz2NumBlocks = %d\n", bz2NumBlocks);
#endif

			// add data to end of block list
			TempBlockListing.dataStart = startByte;
			TempBlockListing.dataSize = 0;
			bz2BlockList.push_back(TempBlockListing);
			bz2NumBlocks++;

			if (currentByte == 0)
			{
				startPointer = memstr(startPointer+1, ret-(startPointer-FileData)-1, bz2Header, strlen(bz2Header));
			}
			else
			{
				startPointer = memstr(startPointer+1, ret-(startPointer-FileData)-1+(strlen(bz2Header)-1), bz2Header, strlen(bz2Header));
			}
		}

		currentByte += ret;
		bytesLeft -= ret;
	} // while

	// use direcdecompress() instead to process 1 bzip2 stream
	if (bz2NumBlocks <= 1)
	{
		if (QuietMode != 1)
			fprintf(stderr, "Switching to no threads mode: only 1 BZIP2 block found.\n");
		return;
	}

	if (FileData != NULL)
		delete [] FileData;
	NumBlocks = bz2NumBlocks;

	// calculate data sizes for each block
	for (int i=0; i < bz2NumBlocks; i++)
	{
		if (i == bz2NumBlocks-1)
		{
			// special case for last block
			bz2BlockList[i].dataSize = IO_data_vec[sourceId].fileSize - bz2BlockList[i].dataStart;
		}
		else if (i == 0)
		{
			// special case for first block
			bz2BlockList[i].dataSize = bz2BlockList[i+1].dataStart;
		}
		else
		{
			// normal case
			bz2BlockList[i].dataSize = bz2BlockList[i+1].dataStart - bz2BlockList[i].dataStart;
		}
#ifdef PBZIP_DEBUG
		fprintf(stderr, " bz2BlockList[%d].dataStart = %" "llu" "\n", i, bz2BlockList[i].dataStart);
		fprintf(stderr, " bz2BlockList[%d].dataSize = %" "llu" "\n", i, bz2BlockList[i].dataSize);
#endif
		//printf("dataSize: %ld\n", bz2BlockList[i].dataSize);
	}

	if(SPBench::memory_source_is_enabled()){
		bytesLeft = IO_data_vec[sourceId].fileSize;
		/*MemReadData = NULL;
		MemWriteData = NULL;
		MemReadData = new char[IO_data_vec[sourceId].fileSize];*/

		for (int i=0; i < bz2NumBlocks; i++)
		{
			// go to start of block position in file
#ifndef WIN32
			int ret = lseek(IO_data_vec[sourceId].hInfile, bz2BlockList[i].dataStart, SEEK_SET);
#else
			int ret = bz2BlockList[i].dataStart;
			LOW_DWORD(ret) = SetFilePointer((HANDLE)_get_osfhandle(hInfile), LOW_DWORD(ret), &HIGH_DWORD(ret), FILE_BEGIN);
#endif
			// read file data
			input_data_inmemory file_data;
			file_data.block = new char[bz2BlockList[i].dataSize];
			ret = read(IO_data_vec[sourceId].hInfile, (char *) file_data.block, bz2BlockList[i].dataSize);
			IO_data_vec[sourceId].MemData.push_back(file_data);
		}
#ifdef PBZIP_DEBUG
		fprintf(stderr, " -> Total Bytes Read: %" "llu" " bytes...\n\n", ret);
#endif
		bytesLeft = IO_data_vec[sourceId].fileSize;
	}
	
	unsigned int sourceItemCounter = 0;
	bool end_of_input = false;
	long source_item_timestamp = current_time_usecs();


	while(1){

		Item item;

		//if last batch included the last item, ends computation
		if(end_of_input == true){
			item.setLastItem();
			sourceQueue.enqueue(item);
			break;
		}

		// frequency control mechanism
		item_frequency_control(source_item_timestamp, sourceFrequency);

		item.timestamp = source_item_timestamp = current_time_usecs();
		unsigned long batch_elapsed_time = source_item_timestamp;
		
		volatile unsigned long latency_op;
		item.timestamp = current_time_usecs();
		if(Metrics::latency_is_enabled()){
			latency_op = current_time_usecs();
		}

		while(1){ //batch loop

			// batching management routines
			if(sourceBatchInterval){
				if(((current_time_usecs() - batch_elapsed_time) / 1000.0) >= sourceBatchInterval) break;
			} else {
				if(item.batch_size >= sourceBatchSize) break;
			}
			if(sourceBatchSize > 1){
				if(item.batch_size >= sourceBatchSize) break;
			}
			
			struct item_data item_data;
			item_data.sourceId = getSourceId();

		// go to start of block position in file
		#ifndef WIN32
			int ret = lseek(IO_data_vec[sourceId].hInfile, bz2BlockList[sourceItemCounter].dataStart, SEEK_SET);
		#else
			int ret = bz2BlockList[sourceItemCounter].dataStart;
			LOW_DWORD(ret) = SetFilePointer((HANDLE)_get_osfhandle(hInfile), LOW_DWORD(ret), &HIGH_DWORD(ret), FILE_BEGIN);
		#endif
			if (ret != bz2BlockList[sourceItemCounter].dataStart)
			{
				fprintf(stderr, "Bzip2: *ERROR: Could not seek to beginning of file [%" "llu" "]!  Skipping...\n", (unsigned long long)ret);
				exit(-1);
			}

			// set buffer size
			item_data.buffSize = bz2BlockList[sourceItemCounter].dataSize;

		#ifdef PBZIP_DEBUG
			fprintf(stderr, " -> Bytes To Read: %" "llu" " bytes...\n", item_data.buffSize);
		#endif

			if (QuietMode != 1)
			{
				// give warning to user if block is larger than 250 million bytes
				if (item_data.buffSize > 250000000)
				{
					fprintf(stderr, "Bzip2:  *WARNING: Compressed block size is large [%" "llu" " bytes].\n", (unsigned long long)item_data.buffSize);
					fprintf(stderr, "          If program aborts, use regular BZIP2 to decompress.\n");
				}
			}
			// allocate memory to read in file
			item_data.FileData = NULL;

			if(SPBench::memory_source_is_enabled()){
				item_data.FileData = IO_data_vec[sourceId].MemData[sourceItemCounter].block;
				item_data.index = sourceItemCounter;
				if (bytesLeft == 0){
					end_of_input = true; 
					break;
				}
				bytesLeft -= item_data.buffSize;
			} else {
				item_data.FileData = new char[item_data.buffSize];
				// make sure memory was allocated properly
				if (item_data.FileData == NULL)
				{
					fprintf(stderr, "Bzip2: *ERROR: Could not allocate memory (FileData)!  Skipping...\n");
					close(IO_data_vec[sourceId].hInfile);
					exit(-1);
				}

				// read file data
				int rret = read(IO_data_vec[sourceId].hInfile, (char *) item_data.FileData, item_data.buffSize);

		#ifdef PBZIP_DEBUG
				fprintf(stderr, " -> Total Bytes Read: %" "llu" " bytes...\n\n", ret);
		#endif

				// check to make sure all the data we expected was read in
				if (rret == 0)
				{	
					if (item_data.FileData != NULL)
						delete [] item_data.FileData;
					end_of_input = true;
					break;
				}
				else if (rret < 0)
				{
					fprintf(stderr, "Bzip2: *ERROR: Could not read from file!  Skipping...\n");
					close(IO_data_vec[sourceId].hInfile);
					if (item_data.FileData != NULL)
						delete [] item_data.FileData;
					exit(-1);
				}

				else if (rret != item_data.buffSize)
				{
					fprintf(stderr, "Bzip2: *ERROR: Could not read enough data from file!  Skipping...\n");
					close(IO_data_vec[sourceId].hInfile);
					if (item_data.FileData != NULL)
						delete [] item_data.FileData;
					exit(-1);
				}
			}
			if(end_of_input) break; // this may not be necessary
			item.item_batch.resize(item.batch_size+1);
			//item_data.index = sourceItemCounter;
			item.item_batch[item.batch_size] = item_data;
			item.batch_size++;
			item.setNotEmpty();
			sourceItemCounter++;
		}

		//if this batch has size 0, ends computation
		if(item.batch_size == 0){
			item.setLastItem();
			sourceQueue.enqueue(item);
			break;
		}

		// frequency control mechanism
		item_frequency_control(item.timestamp, sourceFrequency);

		if(Metrics::latency_is_enabled()){
			item.latency_op[0] = (current_time_usecs() - latency_op);
		}
		
		
		// put item in the output queue
		sourceQueue.enqueue(item);

		// Accumulate the total number of sent batches
		metrics_vec[sourceId].global_batch_counter++;

		// Update the total number of sent items
		metrics_vec[sourceId].global_item_counter = sourceItemCounter;
		
	}
	
	return;
}

void Sink::sink_d(Item &item){

	if(item.empty())
		return;

	volatile unsigned long latency_op;
	if(Metrics::latency_is_enabled()){
		latency_op = current_time_usecs();
	}
	unsigned int num_item = 0;
	while(num_item < item.batch_size){ //batch loop
		item_data item_data;
		item_data = item.item_batch[num_item];

		if(SPBench::memory_source_is_enabled()){
			delete [] IO_data_vec[item.sourceId].MemData[item_data.index].block;
			IO_data_vec[item.sourceId].MemData[item_data.index].block = item_data.CompDecompData;
			IO_data_vec[item.sourceId].MemData[item_data.index].buffSize = item_data.buffSize;

		} else {
			int ret = write(IO_data_vec[item.sourceId].hOutfile, item_data.CompDecompData, item_data.buffSize);

			if (ret <= 0)
			{
				fprintf(stderr, "Bzip2: *ERROR: Could not write to file!  Skipping...\n");
				exit(-1);
			}
		}


		if(!SPBench::memory_source_is_enabled()){
			if (item_data.FileData != NULL)
				delete [] item_data.FileData;
		}
		/*if(!optimized_memory){
			if (item_data.CompDecompData != NULL)
				delete [] item_data.CompDecompData;
		}*/
		num_item++;
	}
	
	metrics_vec[item.sourceId].batches_at_sink_counter++;

	if(Metrics::monitoring_is_enabled()){
		monitor_metrics(item.timestamp, item.sourceId);
	}
	if(Metrics::latency_is_enabled()){

		double current_time_sink = current_time_usecs();
		item.latency_op[2] = (current_time_sink - latency_op);

		volatile unsigned long total_item_latency = (current_time_sink - item.timestamp);
		metrics_vec[item.sourceId].global_latency_acc += total_item_latency; // to compute real time average latency
		
		item_metrics_data latency;
		latency.local_latency = item.latency_op;
		latency.total_latency = total_item_latency;
		latency.item_sink_timestamp = current_time_sink;
		latency.batch_size = item.batch_size;
		metrics_vec[item.sourceId].latency_vector_ns.push_back(latency);
		metrics_vec[item.sourceId].stop_throughput_clock = current_time_sink;
		item.latency_op.clear();
	}
}

int direct_decompress()
{
	decompress();
	return 0;
}

/*
 * Simulate an unconditional read(), reading in data to fill the
 * bsize-sized buffer if it can, even if it means calling read() multiple
 * times. This is needed since pipes and other "special" streams
 * sometimes don't allow reading of arbitrary sized buffers.
 */
ssize_t bufread(int hf, char *buf, size_t bsize)
{
	size_t bufr = 0;
	int ret;
	int rsize = bsize;

	while (1)
	{
		ret = read(hf, buf, rsize);

		if (ret < 0)
			return ret;
		if (ret == 0)
			return bufr;

		bufr += ret;
		if (bufr == bsize)
			return bsize;
		rsize -= ret;
		buf += ret;
	}
}

/*
 *********************************************************
 */
void mySignalCatcher(int n)
{
	struct stat statBuf;
	int ret = 0;

	fprintf(stderr, "\n *Control-C or similar caught, quitting...\n");
#ifdef PBZIP_DEBUG
	fprintf(stderr, " Infile: %s   Outfile: %s\n", sigInFilename, sigOutFilename);
#endif

	// only cleanup files if we did something with them
	if ((sigInFilename == NULL) || (sigOutFilename == NULL))
		exit(1);

	// check to see if input file still exists
	ret = stat(sigInFilename, &statBuf);
	if (ret == 0)
	{
		// only want to remove output file if input still exists
		if (QuietMode != 1)
			fprintf(stderr, "Deleting output file: %s, if it exists...\n", sigOutFilename);
		ret = remove(sigOutFilename);
		if (ret != 0)
			fprintf(stderr, "Bzip2:  *WARNING: Deletion of output file (apparently) failed.\n");
	}
	else
	{
		fprintf(stderr, "Bzip2:  *WARNING: Output file was not deleted since input file no longer exists.\n");
		fprintf(stderr, "Bzip2:  *WARNING: Output file: %s, may be incomplete!\n", sigOutFilename);
	}

	exit(1);
}

/*
 *********************************************************
 This function will search the array pointed to by
 searchBuf[] for the string searchString[] and return
 a pointer to the start of the searchString[] if found
 otherwise return NULL if not found.
 */
char *memstr(char *searchBuf, int searchBufSize, char *searchString, int searchStringSize)
{
	int i;

	for (i=0; i < searchBufSize; i++)
	{
		if ((searchBufSize - i) < searchStringSize)
			break;

		if ( searchBuf[i] == searchString[0] && 
				memcmp(searchBuf+i, searchString, searchStringSize) == 0 ) 
		{
			return &searchBuf[i];
		}	
	}

	return NULL;
}

/*
 *********************************************************
 Much of the code in this function is taken from bzip2.c
 */
int testBZ2ErrorHandling(int bzerr, BZFILE* bzf, int streamNo)
{
	int bzerr_dummy;

	BZ2_bzReadClose(&bzerr_dummy, bzf);
	switch (bzerr)
	{
		case BZ_CONFIG_ERROR:
			fprintf(stderr, "Bzip2: *ERROR: Integers are not the right size for libbzip2. Aborting!\n");
			exit(3);
			break;
		case BZ_IO_ERROR:
			fprintf(stderr, "Bzip2: *ERROR: Integers are not the right size for libbzip2. Aborting!\n");
			return 1;
			break;
		case BZ_DATA_ERROR:
			fprintf(stderr,	"Bzip2: *ERROR: Data integrity (CRC) error in data!  Skipping...\n");
			return -1;
			break;
		case BZ_MEM_ERROR:
			fprintf(stderr, "Bzip2: *ERROR: Could NOT allocate enough memory. Aborting!\n");
			return 1;
			break;
		case BZ_UNEXPECTED_EOF:
			fprintf(stderr,	"Bzip2: *ERROR: File ends unexpectedly!  Skipping...\n");
			return -1;
			break;
		case BZ_DATA_ERROR_MAGIC:
			if (streamNo == 1)
			{
				fprintf(stderr, "Bzip2: *ERROR: Bad magic number (file not created by bzip2)!  Skipping...\n");
				return -1;
			}
			else
			{
				if (QuietMode != 1)
					fprintf(stderr, "Bzip2: *WARNING: Trailing garbage after EOF ignored!\n");
				return 0;
			}
		default:
			fprintf(stderr, "Bzip2: *ERROR: Unexpected error. Aborting!\n");
			exit(3);
	}

	return 0;
}

/*
 *********************************************************
 Much of the code in this function is taken from bzip2.c
 */
int testCompressedData(char *fileName)
{
	FILE *zStream = NULL;
	int ret = 0;

	BZFILE* bzf = NULL;
	unsigned char obuf[5000];
	unsigned char unused[BZ_MAX_UNUSED];
	unsigned char *unusedTmp;
	int bzerr, nread, streamNo;
	int nUnused;
	int i;

	nUnused = 0;
	streamNo = 0;

	// see if we are using stdin or not
	if (strcmp(fileName, "-") != 0) 
	{
		// open the file for reading
		zStream = fopen(fileName, "rb");
		if (zStream == NULL)
		{
			fprintf(stderr, "Bzip2: *ERROR: Could not open input file [%s]!  Skipping...\n", fileName);
			return -1;
		}
	}
	else
		zStream = stdin;

	// check file stream for errors
	if (ferror(zStream))
	{
		fprintf(stderr, "Bzip2: *ERROR: Problem with stream of file [%s]!  Skipping...\n", fileName);
		if (zStream != stdin)
			fclose(zStream);
		return -1;
	}

	// loop until end of file
	while(true)
	{
		bzf = BZ2_bzReadOpen(&bzerr, zStream, Verbosity, 0, unused, nUnused);
		if (bzf == NULL || bzerr != BZ_OK)
		{
			ret = testBZ2ErrorHandling(bzerr, bzf, streamNo);
			if (zStream != stdin)
				fclose(zStream);
			return ret;
		}

		streamNo++;

		while (bzerr == BZ_OK)
		{
			nread = BZ2_bzRead(&bzerr, bzf, obuf, sizeof(obuf));
			if (bzerr == BZ_DATA_ERROR_MAGIC)
			{
				ret = testBZ2ErrorHandling(bzerr, bzf, streamNo);
				if (zStream != stdin)
					fclose(zStream);
				return ret;
			}
		}
		if (bzerr != BZ_STREAM_END)
		{
			ret = testBZ2ErrorHandling(bzerr, bzf, streamNo);
			if (zStream != stdin)
				fclose(zStream);
			return ret;
		}

		BZ2_bzReadGetUnused(&bzerr, bzf, (void**)(&unusedTmp), &nUnused);
		if (bzerr != BZ_OK)
		{
			fprintf(stderr, "Bzip2: *ERROR: Unexpected error. Aborting!\n");
			exit(3);
		}

		for (i = 0; i < nUnused; i++)
			unused[i] = unusedTmp[i];

		BZ2_bzReadClose(&bzerr, bzf);
		if (bzerr != BZ_OK)
		{
			fprintf(stderr, "Bzip2: *ERROR: Unexpected error. Aborting!\n");
			exit(3);
		}

		// check to see if we are at the end of the file
		if (nUnused == 0)
		{
			int c = fgetc(zStream);
			if (c == EOF)
				break;
			else
				ungetc(c, zStream);
		}
	}

	// check file stream for errors
	if (ferror(zStream))
	{
		fprintf(stderr, "Bzip2: *ERROR: Problem with stream of file [%s]!  Skipping...\n", fileName);
		if (zStream != stdin)
			fclose(zStream);
		return -1;
	}

	// close file
	ret = fclose(zStream);
	if (ret == EOF)
	{
		fprintf(stderr, "Bzip2: *ERROR: Problem closing file [%s]!  Skipping...\n", fileName);
		return -1;
	}

	return 0;
}

/*
 *********************************************************
 */
int getFileMetaData(char *fileName)
{
	// get the file meta data and store it in the global structure
	return stat(fileName, &fileMetaData);
}

/*
 *********************************************************
 */
int writeFileMetaData(char *fileName)
{
	int ret = 0;
	struct utimbuf uTimBuf;

	// store file times in structure
	uTimBuf.actime = fileMetaData.st_atime;
	uTimBuf.modtime = fileMetaData.st_mtime;

	// update file with stored file permissions
	ret = chmod(fileName, fileMetaData.st_mode);
	if (ret != 0)
		return ret;

	// update file with stored file access and modification times
	ret = utime(fileName, &uTimBuf);
	if (ret != 0)
		return ret;

	// update file with stored file ownership (if access allows)
#ifndef WIN32
	ret = chown(fileName, fileMetaData.st_uid, fileMetaData.st_gid);
#endif

	return 0;
}

/*
 *********************************************************
 */
int detectCPUs()
{
	int ncpu;

	// Set default to 1 in case there is no auto-detect
	ncpu = 1;

	// Autodetect the number of CPUs on a box, if available
#if defined(__APPLE__)
	size_t len = sizeof(ncpu);
	int mib[2];
	mib[0] = CTL_HW;
	mib[1] = HW_NCPU;
	if (sysctl(mib, 2, &ncpu, &len, 0, 0) < 0 || len != sizeof(ncpu))
		ncpu = 1;
#elif defined(_SC_NPROCESSORS_ONLN)
	ncpu = sysconf(_SC_NPROCESSORS_ONLN);
#endif

	// Ensure we have at least one processor to use
	if (ncpu < 1)
		ncpu = 1;

	return ncpu;
}


/*
 *********************************************************
 */
void banner()
{
	fprintf(stderr, "N-sources SPBench version of bzip2 - by: Adriano Marques Garcia\n");
	fprintf(stderr, "Parallel BZIP2 v1.0.5 - by: Jeff Gilchrist [http://compression.ca]\n");
	fprintf(stderr, "[Jan. 08, 2009]             (uses libbzip2 by Julian Seward)\n");

	return;
}
/*
 *********************************************************
 */
void usage(char* progname, const char *reason)
{
	banner();

	if (strncmp(reason, "HELP", 4) == 0)
		fprintf(stderr, "\n");
	else
		fprintf(stderr, "\nInvalid command line: %s.  Aborting...\n\n", reason);

#ifndef PBZIP_NO_LOADAVG
	fprintf(stderr, "Usage: %s [-1 .. -9] [-b#cdfhklp#qrtVz] <filename> <filename2> <filenameN>\n", progname);
#else
	fprintf(stderr, "Usage: %s [-1 .. -9] [-b#cdfhkp#qrtVz] <filename> <filename2> <filenameN>\n", progname);
#endif
	fprintf(stderr, " -b#      : where # is the number of itens per batch (default 1)\n");
	fprintf(stderr, " -B#      : where # is the time size of the batch (default 0)\n");
	fprintf(stderr, " -Q#      : where # is the file block size in 100k (default 9 = 900k)\n");
	fprintf(stderr, " -t#      : where # is the number of threads (default");
	fprintf(stderr, " -c       : output to standard out (stdout)\n");
	fprintf(stderr, " -d       : decompress file\n");
	fprintf(stderr, " -k       : read entire input file into memory first\n");
	fprintf(stderr, " -l       : print average latency results\n");
	fprintf(stderr, " -f       : store individual latency values into a file\n");
	fprintf(stderr, " -x       : print average throughput results\n");
	fprintf(stderr, " -u       : print memory consumption results generated by UPL library\n");
	fprintf(stderr, " -X       : overwrite existing output file\n");
	fprintf(stderr, " -h       : print this help message\n");
	//fprintf(stderr, " -k       : keep input file, don't delete\n");
#ifndef PBZIP_NO_LOADAVG
	fprintf(stderr, " -l       : load average determines max number processors to use\n");
#endif
	fprintf(stderr, " -P#      : where # is the number of processors (default)");
	fprintf(stderr, " -p#      : where # is the frequency pattern defined as = <pattern,period,min,max>");
#if defined(_SC_NPROCESSORS_ONLN) || defined(__APPLE__)
	fprintf(stderr, ": autodetect [%d])\n", detectCPUs());
#else
	fprintf(stderr, " 2)\n");
#endif
	fprintf(stderr, " -q       : quiet mode (default)\n");
	fprintf(stderr, " -r       : read entire input file into RAM and split between processors\n");
	//fprintf(stderr, " -t       : <number_of_threads>\n");
	//fprintf(stderr, " -t       : test compressed file integrity\n");
	fprintf(stderr, " -v       : verbose mode\n");
	fprintf(stderr, " -V       : display version info for Bzip2 then exit\n");
	fprintf(stderr, " -z       : compress file (default)\n");
	fprintf(stderr, " -1 .. -9 : set BWT block size to 100k .. 900k (default 900k)\n\n");
	fprintf(stderr, "Example: Bzip2 -p4 -r -5 myfile.tar second*.txt\n");
	fprintf(stderr, "Example: tar cf myfile.tar.bz2 --use-compress-prog=Bzip2 dir_to_compress/\n");
	fprintf(stderr, "Example: Bzip2 -d myfile.tar.bz2\n\n");
	exit(-1);
}

/*
 *********************************************************
 */
int bzip2_main(int argc, char* argv[])
{
	//pthread_t output;
	char **FileList = NULL;
	char *InFilename = NULL;
	char *progName = NULL;
	char *progNamePos = NULL;
	char bz2Header[] = {"BZh91AY&SY"};  // using 900k block size
	char bz2HeaderZero[] = { 0x42, 0x5A, 0x68, 0x39, 0x17, 0x72, 0x45, 0x38, 0x50, static_cast<char>(0x90), 0x00, 0x00, 0x00, 0x00 };
	//char OutFilename[2048];
	char cmdLineTemp[2048];
	char tmpBuff[50];
	char stdinFile[2] = {"-"};
	struct timeval tvStartTime;
	struct timeval tvStopTime;
#ifndef WIN32
	struct timezone tz;
	double loadAverage = 0.0;
	double loadAvgArray[3];
	int useLoadAverage = 0;
	int numCPUtotal = 0;
	int numCPUidle = 0;
#else
	SYSTEMTIME systemtime;
	LARGE_INTEGER filetime;
	LARGE_INTEGER fileSize_temp;
	HANDLE hInfile_temp;
#endif
	double timeCalc = 0.0;
	double timeStart = 0.0;
	double timeStop = 0.0;
	OFF_T fileSize = 0;
	size_t size;
	int cmdLineTempCount = 0;
	int readEntireFile = 0;
	int zeroByteFile = 0;
	int hInfile = -1;
	int hOutfile = -1;
	int numBlocks = 0;
	//int blockSize = 9*100000;
	int decompress = 0;
	int testFile = 0;
	int errLevel = 0;
	int noThreads = 0;
	int keep = 0;
	int force = 0;
	int ret = 0;
	int fileLoop;
	int i, j, k;

	int OutputStdOut = 0;

	// get current time for benchmark reference
#ifndef WIN32
	gettimeofday(&tvStartTime, &tz);
#else
	GetSystemTime(&systemtime);
	SystemTimeToFileTime(&systemtime, (FILETIME *)&filetime);
	tvStartTime.tv_sec = filetime.QuadPart / 10000000;
	tvStartTime.tv_usec = (filetime.QuadPart - (LONGLONG)tvStartTime.tv_sec * 10000000) / 10;
#endif


	// check to see if we are likely being called from TAR
	if (argc < 2)
	{
		OutputStdOut = 1;
		keep = 1;
	}

	SPBench::bench_path = argv[0];
	
	// get program name to determine if decompress mode should be used
	progName = argv[0];
	for (progNamePos = argv[0]; progNamePos[0] != '\0'; progNamePos++)
	{
		if (progNamePos[0] == PATH_SEP)
			progName = progNamePos + 1;
	}
	if ((strstr(progName, "unzip") != 0) || (strstr(progName, "UNZIP") != 0))
	{
		decompress = 1;
	}
	if ((strstr(progName, "zcat") != 0) || (strstr(progName, "ZCAT") != 0))
	{
		decompress = OutputStdOut = keep = 1; 
	}

	FileListCount = 0;
	FileList = new char *[argc];
	if (FileList == NULL)
	{
		fprintf(stderr, "Bzip2: *ERROR: Not enough memory!  Aborting...\n");
		return 1;
	}

	numCPU = detectCPUs();

#ifndef WIN32
	numCPUtotal = numCPU;
#endif

	// parse command line switches
	for (i=1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (argv[i][1] == '\0') 
			{
				// support "-" as a filename
				FileList[FileListCount] = argv[i];
				FileListCount++;
				continue;
			}
			else if (argv[i][1] == '-')
			{
				// get command line options with "--"
				if (strcmp(argv[i], "--best") == 0)
				{
					BWTblockSize = 9;
				}
				else if (strcmp(argv[i], "--decompress") == 0)
				{
					decompress = 1;
				}
				else if (strcmp(argv[i], "--compress") == 0)
				{
					decompress = 0;
				}
				else if (strcmp(argv[i], "--fast") == 0)
				{
					BWTblockSize = 1;
				}
				else if (strcmp(argv[i], "--force") == 0)
				{
					force = 1; ForceOverwrite = 1;
				}
				else if (strcmp(argv[i], "--help") == 0)
				{
					usage(argv[0], "HELP");
				}
				else if (strcmp(argv[i], "--keep") == 0)
				{
					keep = 1;
				}
				else if (strcmp(argv[i], "--license") == 0)
				{
					usage(argv[0], "HELP");
				}
				else if (strcmp(argv[i], "--quiet") == 0)
				{
					QuietMode = 1;
				}
				else if (strcmp(argv[i], "--stdout") == 0)
				{
					OutputStdOut = 1; keep = 1;
				}
				else if (strcmp(argv[i], "--test") == 0)
				{
					testFile = 1;
				}
				else if (strcmp(argv[i], "--verbose") == 0)
				{
					QuietMode = 0;
				}
				else if (strcmp(argv[i], "--version") == 0)
				{
					banner(); exit(0);
				}

				continue;
			}
#ifdef PBZIP_DEBUG
			fprintf(stderr, "argv[%d]: %s   Len: %d\n", i, argv[i], strlen(argv[i]));
#endif
			// get command line options with single "-"
			// check for multiple switches grouped together
			for (j=1; argv[i][j] != '\0'; j++)
			{
				switch (argv[i][j])
				{
					case 'P': k = j+1; cmdLineTempCount = 0; strcpy(cmdLineTemp, "2");
						  while (argv[i][k] != '\0' && k < sizeof(cmdLineTemp))
						  {
							  // no more numbers, finish
							  if ((argv[i][k] < '0') || (argv[i][k] > '9'))
								  break;
							  k++;
							  cmdLineTempCount++;
						  }
						  if (cmdLineTempCount == 0)
							  usage(argv[0], "Cannot parse -p argument");
						  strncpy(cmdLineTemp, argv[i]+j+1, cmdLineTempCount);
						  numCPU = atoi(cmdLineTemp);
						  if (numCPU > 4096)
						  {
							  fprintf(stderr,"Bzip2: *ERROR: Maximal number of supported processors is 4096!  Aborting...\n");
							  return 1;
						  }
						  else if (numCPU < 1)
						  {
							  fprintf(stderr,"Bzip2: *ERROR: Minimum number of supported processors is 1!  Aborting...\n");
							  return 1;
						  }
						  j += cmdLineTempCount;
#ifdef PBZIP_DEBUG
						  fprintf(stderr, "-p%d\n", numCPU);
#endif
						  break;
					case 't': k = j+1; cmdLineTempCount = 0; strcpy(cmdLineTemp, "2");
						  while (argv[i][k] != '\0' && k < sizeof(cmdLineTemp))
						  {
							  // no more numbers, finish
							  if ((argv[i][k] < '0') || (argv[i][k] > '9'))
								  break;
							  k++;
							  cmdLineTempCount++;
						  }
						  if (cmdLineTempCount == 0)
							  usage(argv[0], "Cannot parse -t argument");
						  strncpy(cmdLineTemp, argv[i]+j+1, cmdLineTempCount);
						  nthreads = atoi(cmdLineTemp);
						  if (nthreads < 1)
						  {
							  fprintf(stderr,"Bzip2: *ERROR: Minimum number of supported processors is 1!  Aborting...\n");
							  return 1;
						  }
						  j += cmdLineTempCount;
#ifdef PBZIP_DEBUG
						  fprintf(stderr, "-t%d\n", nthreads);
#endif
						  break;
					//batch size
					case 'b': k = j+1; cmdLineTempCount = 0; strcpy(cmdLineTemp, "2");
						  while (argv[i][k] != '\0' && k < sizeof(cmdLineTemp))
						  {
							  // no more numbers, finish
							  if ((argv[i][k] < '0') || (argv[i][k] > '9'))
								  break;
							  k++;
							  cmdLineTempCount++;
						  }
						  if (cmdLineTempCount == 0)
							  usage(argv[0], "Cannot parse -b argument");
						  strncpy(cmdLineTemp, argv[i]+j+1, cmdLineTempCount);
						  SPBench::setBatchSize(atoi(cmdLineTemp));
						  if (SPBench::getBatchSize() < 1)
						  {
							  fprintf(stderr,"Bzip2: *ERROR: Minimum batch size is 1!  Aborting...\n");
							  return 1;
						  }
						  j += cmdLineTempCount;
#ifdef PBZIP_DEBUG
						  fprintf(stderr, "-b%d\n", batch_size);
#endif
						  break;
					// batch interval
					case 'B': k = j + 1; cmdLineTempCount = 0; strcpy(cmdLineTemp, "2");
						while (argv[i][k] != '\0' && k < sizeof(cmdLineTemp))
						{
							// no more numbers, finish
							if ((argv[i][k] < '0') || (argv[i][k] > '9'))
								break;
							k++;
							cmdLineTempCount++;
						}
						if (cmdLineTempCount == 0)
							usage(argv[0], "Cannot parse -B argument");
						strncpy(cmdLineTemp, argv[i] + j + 1, cmdLineTempCount);
						SPBench::setBatchInterval(atoi(cmdLineTemp));
						if (SPBench::getBatchInterval() <= 0)
						{
							fprintf(stderr, "Bzip2: *ERROR: Minimum batch time size is 0!  Aborting...\n");
							return 1;
						}
						j += cmdLineTempCount;
	#ifdef PBZIP_DEBUG
						fprintf(stderr, "-b%d\n", batch_size);
	#endif
						break;
					// Frequency
					case 'F': k = j+1; cmdLineTempCount = 0; strcpy(cmdLineTemp, "2");
						  while (argv[i][k] != '\0' && k < sizeof(cmdLineTemp))
						  {
							  // no more numbers, finish
							  if ((argv[i][k] < '0') || (argv[i][k] > '9'))
								  break;
							  k++;
							  cmdLineTempCount++;
						  }
						  if (cmdLineTempCount == 0)
							  usage(argv[0], "Cannot parse -F argument");
						  strncpy(cmdLineTemp, argv[i]+j+1, cmdLineTempCount);
						  SPBench::setFrequency(atoi(cmdLineTemp));
						  SPBench::enable_memory_source();

						  if (SPBench::getFrequency() < 0)
						  {
							  fprintf(stderr,"Bzip2: *ERROR: Minimum frequency is 0!  Aborting...\n");
							  return 1;
						  }
						  j += cmdLineTempCount;
#ifdef PBZIP_DEBUG
						  fprintf(stderr, "-F%d\n", items_reading_frequency);
#endif
						  break;

					case 'p': k = j + 1; cmdLineTempCount = 0; strcpy(cmdLineTemp, "2");
						while (argv[i][k] != '\0' && k < sizeof(cmdLineTemp))
						{
							k++;
							cmdLineTempCount++;
						}

						strncpy(cmdLineTemp, argv[i] + j + 1, cmdLineTempCount + 1);
						try {
							input_freq_pattern_parser(cmdLineTemp);
						}
						catch (const std::invalid_argument& e) {
							std::cerr << "exception: " << e.what() << std::endl;
							exit(1);
						}
						j += cmdLineTempCount;
						break;

					//block size
					case 'Q': k = j+1; cmdLineTempCount = 0; strcpy(cmdLineTemp, "9"); blockSize = 900000;
						  while (argv[i][k] != '\0' && k < sizeof(cmdLineTemp))
						  {
							  // no more numbers, finish
							  if ((argv[i][k] < '0') || (argv[i][k] > '9'))
								  break;
							  k++;
							  cmdLineTempCount++;
						  }
						  if (cmdLineTempCount == 0)
							  usage(argv[0], "Cannot parse file block size");
						  strncpy(cmdLineTemp, argv[i]+j+1, cmdLineTempCount);
						  blockSize = atoi(cmdLineTemp)*100000;
						  if ((blockSize < 100000) || (blockSize > 1000000000))
						  {
							  fprintf(stderr,"Bzip2: *ERROR: File block size Min: 100k and Max: 10000k!  Aborting...\n");
							  return 1;
						  }
						  j += cmdLineTempCount;
#ifdef PBZIP_DEBUG
						  fprintf(stderr, "-b%d\n", blockSize);
#endif
						  break;
					case 'm': k = j+1; cmdLineTempCount = 0; strcpy(cmdLineTemp, "2");
						  while (argv[i][k] != '\0' && k < sizeof(cmdLineTemp))
						  {
							  // no more numbers, finish
							  if ((argv[i][k] < '0') || (argv[i][k] > '9'))
								  break;
							  k++;
							  cmdLineTempCount++;
						  }
						  if (cmdLineTempCount == 0)
							  usage(argv[0], "Cannot parse -m argument");
						  strncpy(cmdLineTemp, argv[i]+j+1, cmdLineTempCount);
						  Metrics::set_monitoring_time_interval(atoi(cmdLineTemp));
						  Metrics::enable_monitoring();
						  if (Metrics::get_monitoring_time_interval() > 100000)
						  {
							  fprintf(stderr,"Bzip2: *ERROR: Maximal interval time is 100000 (100 seconds)!  Aborting...\n");
							  return 1;
						  }
						  j += cmdLineTempCount;
#ifdef PBZIP_DEBUG
						  fprintf(stderr, "-t%d\n", monitoring_time_interval);
#endif
						  break;
					case 'u': k = j+1; cmdLineTempCount = 0; strcpy(cmdLineTemp, "2");
						  while (argv[i][k] != '\0' && k < sizeof(cmdLineTemp))
						  {

							  k++;
							  cmdLineTempCount++;
						  }

						  strncpy(cmdLineTemp, argv[i]+j+1, cmdLineTempCount+1);

						  SPBench::setArg(cmdLineTemp);
						  j += cmdLineTempCount;
						  break;
					case 'h': usage(argv[0], "HELP"); break;
					case 'd': decompress = 1; break;
					case 'k': SPBench::enable_memory_source(); break; // optimized_memory = true;
					//case 'm': monitoring_time_interval = atoi(optarg); enable_monitoring = true; break;
					//case 'o': optimized_memory = true; memory_source = true; break;
					case 'l': Metrics::enable_print_latency(); break;
					case 'f': Metrics::enable_latency_to_file(); break;
					case 'x': Metrics::enable_throughput(); break;
					case 'r': Metrics::enable_upl(); break;
					case 'c': OutputStdOut = 1; break;
					case 'X': force = 1; ForceOverwrite = 1; break;
					case 'C': keep = 1; break;
						  //#ifndef PBZIP_NO_LOADAVG
						  //					case 'l': useLoadAverage = 1; break;
						  //#endif
					case 'L': banner(); exit(0); break;
					case 'q': QuietMode = 1; break;
					case 'i': break;
					case 'R': readEntireFile = 1; break;
						  //case 't': testFile = 1; break;
						  //case 't':  = 1; break;
					case 'v': QuietMode = 0; break;
					case 'V': banner(); exit(0); break;
					case 'z': decompress = 0; break;

					case '1': BWTblockSize = 1; break;
					case '2': BWTblockSize = 2; break;
					case '3': BWTblockSize = 3; break;
					case '4': BWTblockSize = 4; break;
					case '5': BWTblockSize = 5; break;
					case '6': BWTblockSize = 6; break;
					case '7': BWTblockSize = 7; break;
					case '8': BWTblockSize = 8; break;
					case '9': BWTblockSize = 9; break;
				}
			}
		}
		else
		{
			// add filename to list for processing FileListCount
			FileList[FileListCount] = argv[i];
			FileListCount++;
		}
	} /* for */

	set_operators_name();

	if (FileListCount == 0)
	{
		if (testFile == 1)
		{
#ifndef WIN32
			if (isatty(fileno(stdin)))
#else
				if (_isatty(_fileno(stdin)))
#endif
				{
					fprintf(stderr,"Bzip2: *ERROR: Won't read compressed data from terminal.  Aborting!\n");
					fprintf(stderr,"Bzip2: For help type: %s -h\n", argv[0]);
					return 1;
				}
			// expecting data from stdin
			FileList[FileListCount] = stdinFile;
			FileListCount++;
		}
		else if (OutputStdOut == 1)
		{
#ifndef WIN32
			if (isatty(fileno(stdout)))
#else
				if (_isatty(_fileno(stdout)))
#endif
				{
					fprintf(stderr,"Bzip2: *ERROR: Won't write compressed data to terminal.  Aborting!\n");
					fprintf(stderr,"Bzip2: For help type: %s -h\n", argv[0]);
					return 1;
				}
			// expecting data from stdin
			FileList[FileListCount] = stdinFile;
			FileListCount++;
		}
		else if ((decompress == 1) && (argc == 2))
		{
#ifndef WIN32
			if (isatty(fileno(stdin)))
#else
				if (_isatty(_fileno(stdin)))
#endif
				{
					fprintf(stderr,"Bzip2: *ERROR: Won't read compressed data from terminal.  Aborting!\n");
					fprintf(stderr,"Bzip2: For help type: %s -h\n", argv[0]);
					return 1;
				}
			// expecting data from stdin via TAR
			OutputStdOut = 1;
			keep = 1;
			FileList[FileListCount] = stdinFile;
			FileListCount++;
		}
		else
			usage(argv[0], "Not enough files given");
	}

	if (QuietMode != 1)
	{
		// display program banner
		banner();

		// do sanity check to make sure integers are the size we expect
#ifdef PBZIP_DEBUG
		fprintf(stderr, "off_t size: %d    uint size: %d\n", sizeof(OFF_T), sizeof(unsigned int));
#endif
		if (sizeof(OFF_T) <= 4)
		{
			fprintf(stderr, "\nBzip2: *WARNING: off_t variable size only %lu bits!\n", sizeof(OFF_T)*8);
			if (decompress == 1)
				fprintf(stderr, " You will only able to uncompress files smaller than 2GB in size.\n\n");
			else
				fprintf(stderr, " You will only able to compress files smaller than 2GB in size.\n\n");
		}
	}

	// Calculate number of processors to use based on load average if requested
#ifndef PBZIP_NO_LOADAVG
	if (useLoadAverage == 1)
	{
		// get current load average
		ret = getloadavg(loadAvgArray, 3);
		if (ret != 3)
		{
			loadAverage = 0.0;
			useLoadAverage = 0;
			if (QuietMode != 1)
				fprintf(stderr, "Bzip2:  *WARNING: Could not get load average!  Using requested processors...\n");
		}
		else
		{
#ifdef PBZIP_DEBUG
			fprintf(stderr, "Load Avg1: %f  Avg5: %f  Avg15: %f\n", loadAvgArray[0], loadAvgArray[1], loadAvgArray[2]);
#endif
			// use 1 min load average to adjust number of processors used
			loadAverage = loadAvgArray[0];	// use [1] for 5 min average and [2] for 15 min average
			// total number processors minus load average rounded up
			numCPUidle = numCPUtotal - (int)(loadAverage + 0.5);
			// if user asked for a specific # processors and they are idle, use all requested
			// otherwise give them whatever idle processors are available
			if (numCPUidle < numCPU)
				numCPU = numCPUidle;
			if (numCPU < 1)
				numCPU = 1;
		}
	}
#endif

	// setup signal handling
	sigInFilename = NULL;
	sigOutFilename = NULL;
	signal(SIGINT,  mySignalCatcher);
	signal(SIGTERM, mySignalCatcher);
#ifndef WIN32
	signal(SIGHUP,  mySignalCatcher);
#endif

	if (numCPU < 1)
		numCPU = 1;

	// display global settings
	if (QuietMode != 1)
	{
		if (testFile != 1)
		{
			fprintf(stderr, "\n         # CPUs: %d\n", numCPU);
#ifndef PBZIP_NO_LOADAVG
			if (useLoadAverage == 1)
				fprintf(stderr, "   Load Average: %.2f\n", loadAverage);
#endif
			if (decompress != 1)
			{
				fprintf(stderr, " BWT Block Size: %d00k\n", BWTblockSize);
				if (blockSize < 100000)
					fprintf(stderr, "File Block Size: %d bytes\n", blockSize);
				else
					fprintf(stderr, "File Block Size: %dk\n", blockSize/1000);
			}
		}
		fprintf(stderr, "-------------------------------------------\n");
	}

	/*	MemMutex = new pthread_mutex_t;
	// make sure memory was allocated properly
	if (MemMutex == NULL)
	{
	fprintf(stderr, "Bzip2: *ERROR: Could not allocate memory (MemMutex)!  Aborting...\n");
	return 1;
	}
	pthread_mutex_init(MemMutex, NULL);*/

	/* -------------------------------- */

	// process all files
	for (fileLoop=0; fileLoop < FileListCount; fileLoop++)
	{
		IO_data_struct in_data;
		fileSize = 0;

		// set input filename
		InFilename = FileList[fileLoop];

		// test file for errors if requested
		if (testFile != 0)
		{
			if (QuietMode != 1)
			{
				fprintf(stderr, "      File #: %d of %d\n", fileLoop+1, FileListCount);
				if (strcmp(InFilename, "-") != 0) 
					fprintf(stderr, "     Testing: %s\n", InFilename);
				else
					fprintf(stderr, "     Testing: <stdin>\n");
			}
			ret = testCompressedData(InFilename);
			if (ret > 0)
				return ret;
			else if (ret == 0)
			{
				if (QuietMode != 1)
					fprintf(stderr, "        Test: OK\n");
			}
			else
				errLevel = 2;

			if (QuietMode != 1)
				fprintf(stderr, "-------------------------------------------\n");
			continue;
		}

		// set ouput filename
		strncpy(in_data.OutFilename, FileList[fileLoop], 2040);
		if ((decompress == 1) && (strcmp(InFilename, "-") != 0))
		{
			// check if input file is a valid .bz2 compressed file
			hInfile = open(InFilename, O_RDONLY | O_BINARY);
			// check to see if file exists before processing
			if (hInfile == -1)
			{
				fprintf(stderr, "Bzip2: *ERROR: File [%s] NOT found!  Skipping...\n", InFilename);
				fprintf(stderr, "-------------------------------------------\n");
				errLevel = 1;
				continue;
			}
			memset(tmpBuff, 0, sizeof(tmpBuff));
			size = read(hInfile, tmpBuff, strlen(bz2Header)+1);
			close(hInfile);
			if ((size == (size_t)(-1)) || (size < strlen(bz2Header)+1))
			{
				fprintf(stderr, "Bzip2: *ERROR: File [%s] is NOT a valid bzip2!  Skipping...\n", InFilename);
				fprintf(stderr, "-------------------------------------------\n");
				errLevel = 1;
				continue;
			}
			else
			{
				// make sure start of file has valid bzip2 header
				if (memstr(tmpBuff, 4, bz2Header, 3) == NULL)
				{
					fprintf(stderr, "Bzip2: *ERROR: File [%s] is NOT a valid bzip2!  Skipping...\n", InFilename);
					fprintf(stderr, "-------------------------------------------\n");
					errLevel = 1;
					continue;
				}
				// skip 4th char which differs depending on BWT block size used
				if (memstr(tmpBuff+4, size-4, bz2Header+4, strlen(bz2Header)-4) == NULL)
				{
					// check to see if this is a special 0 byte file
					if (memstr(tmpBuff+4, size-4, bz2HeaderZero+4, strlen(bz2Header)-4) == NULL)
					{
						fprintf(stderr, "Bzip2: *ERROR: File [%s] is NOT a valid bzip2!  Skipping...\n", InFilename);
						fprintf(stderr, "-------------------------------------------\n");
						errLevel = 1;
						continue;
					}
#ifdef PBZIP_DEBUG
					fprintf(stderr, "** ZERO byte compressed file detected\n");
#endif
				}
				// set block size for decompression
				if ((tmpBuff[3] >= '1') && (tmpBuff[3] <= '9'))
					BWTblockSizeChar = tmpBuff[3];
				else
				{
					fprintf(stderr, "Bzip2: *ERROR: File [%s] is NOT a valid bzip2!  Skipping...\n", InFilename);
					fprintf(stderr, "-------------------------------------------\n");
					errLevel = 1;
					continue;
				}
			}

			// check if filename ends with .bz2
			if (strncasecmp(&in_data.OutFilename[strlen(in_data.OutFilename)-4], ".bz2", 4) == 0)
			{
				// remove .bz2 extension
				in_data.OutFilename[strlen(in_data.OutFilename)-4] = '\0';
			}
			else
			{
				// add .out extension so we don't overwrite original file
				strcat(in_data.OutFilename, ".out");
			}
		} // decompress == 1
		else
		{
			// check input file to make sure its not already a .bz2 file
			if (strncasecmp(&InFilename[strlen(InFilename)-4], ".bz2", 4) == 0)
			{
				fprintf(stderr, "Bzip2: *ERROR: Input file [%s] already has a .bz2 extension!  Skipping...\n", InFilename);
				fprintf(stderr, "-------------------------------------------\n");
				errLevel = 1;
				continue;
			}
			strcat(in_data.OutFilename, ".bz2");
		}

		// setup signal handling filenames
		in_data.sigInFilename = InFilename;
		in_data.sigOutFilename = in_data.OutFilename;

		if (strcmp(InFilename, "-") != 0) 
		{
			struct stat statbuf;
			// read file for compression
			hInfile = open(InFilename, O_RDONLY | O_BINARY);
			// check to see if file exists before processing
			if (hInfile == -1)
			{
				fprintf(stderr, "Bzip2: *ERROR: File [%s] NOT found!  Skipping...\n", InFilename);
				fprintf(stderr, "-------------------------------------------\n");
				errLevel = 1;
				continue;
			}

			// get some information about the file
			fstat(hInfile, &statbuf);
			// check to make input is not a directory
			if (S_ISDIR(statbuf.st_mode))
			{
				fprintf(stderr, "Bzip2: *ERROR: File [%s] is a directory!  Skipping...\n", InFilename);
				fprintf(stderr, "-------------------------------------------\n");
				errLevel = 1;
				continue;
			}
			// check to make sure input is a regular file
			if (!S_ISREG(statbuf.st_mode))
			{
				fprintf(stderr, "Bzip2: *ERROR: File [%s] is not a regular file!  Skipping...\n", InFilename);
				fprintf(stderr, "-------------------------------------------\n");
				errLevel = 1;
				continue;
			}
			// get size of file
#ifndef WIN32
			fileSize = statbuf.st_size;
#else
			fileSize_temp.LowPart = GetFileSize((HANDLE)_get_osfhandle(hInfile), (unsigned long *)&fileSize_temp.HighPart);
			fileSize = fileSize_temp.QuadPart;
#endif
			// don't process a 0 byte file
			if (fileSize == 0)
			{
				if (decompress == 1)
				{
					fprintf(stderr, "Bzip2: *ERROR: File is of size 0 [%s]!  Skipping...\n", InFilename);
					fprintf(stderr, "-------------------------------------------\n");
					errLevel = 1;
					continue;
				}

				// make sure we handle zero byte files specially
				zeroByteFile = 1;
			}
			else
				zeroByteFile = 0;

			// get file meta data to write to output file
			if (getFileMetaData(InFilename) != 0)
			{
				fprintf(stderr, "Bzip2: *ERROR: Could not get file meta data from [%s]!  Skipping...\n", InFilename);
				fprintf(stderr, "-------------------------------------------\n");
				errLevel = 1;
				continue;
			}
		}
		else
		{
			hInfile = 0;	// stdin
			fileSize = -1;	// fake it
		}

		// check to see if output file exists
		if ((force != 1) && (OutputStdOut == 0))
		{
			hOutfile = open(in_data.OutFilename, O_RDONLY | O_BINARY);
			// check to see if file exists before processing
			if (hOutfile != -1)
			{
				fprintf(stderr, "Bzip2: *ERROR: Output file [%s] already exists!  Use -f to overwrite...\n", in_data.OutFilename);
				fprintf(stderr, "-------------------------------------------\n");
				errLevel = 1;
				close(hOutfile);
				errLevel = 1;
				continue;
			}
		}

		if (readEntireFile == 1)
		{
			if (hInfile == 0) 
			{
				if (QuietMode != 1)
					fprintf(stderr, " *Warning: Ignoring -r switch since input is stdin.\n");
			}
			else
			{
				// determine block size to try and spread data equally over # CPUs
				blockSize = fileSize / numCPU;
			}
		}

		// display per file settings
		if (QuietMode != 1)
		{
			fprintf(stderr, "         File #: %d of %d\n", fileLoop+1, FileListCount);
			fprintf(stderr, "     Input Name: %s\n", hInfile != 0 ? InFilename : "<stdin>");

			if (OutputStdOut == 0)
				fprintf(stderr, "    Output Name: %s\n\n", in_data.OutFilename);
			else
				fprintf(stderr, "    Output Name: <stdout>\n\n");

			if (decompress == 1)
				fprintf(stderr, " BWT Block Size: %c00k\n", BWTblockSizeChar);
			if (strcmp(InFilename, "-") != 0) 
				fprintf(stderr, "     Input Size: %" "llu" " bytes\n", (unsigned long long)fileSize);
		}

		if (decompress == 1)
		{
			numBlocks = 0;
			// Do not use threads if we only have 1 CPU or small files
			//if ((numCPU == 1) || (fileSize < 10000))
			if (fileSize < 10000)
				noThreads = 1;
			else
				noThreads = 0;
			// for now use no threads method for uncompressing from stdin
			if (strcmp(InFilename, "-") == 0)
				noThreads = 1;
		}
		else
		{
			if (fileSize > 0) 
			{
				// calculate the # of blocks of data
				numBlocks = (fileSize + blockSize - 1) / blockSize;
				// Do not use threads for small files where we only have 1 block to process
				// or if we only have 1 CPU
				if ((numBlocks == 1))// || (numCPU == 1))
					noThreads = 1;
				else
					noThreads = 0;
			} 
			else 
			{
				// Simulate a "big" number of buffers. Will need to resize it later
				numBlocks = 10000;
			}

			// write special compressed data for special 0 byte input file case
			if (zeroByteFile == 1)
			{
				hOutfile = 1;
				// write to file instead of stdout
				if (OutputStdOut == 0)
				{
					hOutfile = open(in_data.OutFilename, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, FILE_MODE);
					// check to see if file creation was successful
					if (hOutfile == -1)
					{
						fprintf(stderr, "Bzip2: *ERROR: Could not create output file [%s]!\n", in_data.OutFilename);
						close(hOutfile);
						errLevel = 1;
						continue;
					}
				}
				// write data to the output file
				ret = write(hOutfile, bz2HeaderZero, sizeof(bz2HeaderZero));
				if (OutputStdOut == 0)
					close(hOutfile);
				if (ret != sizeof(bz2HeaderZero))
				{
					fprintf(stderr, "Bzip2: *ERROR: Could not write to file!  Skipping...\n");
					fprintf(stderr, "-------------------------------------------\n");
					errLevel = 1;
					continue;
				}
				if (QuietMode != 1)
				{
					fprintf(stderr, "    Output Size: %" "llu" " bytes\n", (unsigned long long)sizeof(bz2HeaderZero));
					fprintf(stderr, "-------------------------------------------\n");
				}
				continue;
			}
		}
#ifdef PBZIP_DEBUG
		fprintf(stderr, "# Blocks: %d\n", numBlocks);
#endif

		in_data.InFilename = InFilename;
		in_data.fileSize = fileSize;
		in_data.hInfile = hInfile;
		in_data.OutputStdOut = OutputStdOut;
		in_data.hOutfile = hOutfile;

		IO_data_vec.push_back(in_data);

		// set global variable
		NumBlocks = numBlocks;
	}

	if (decompress == 1)
	{
		//if(Metrics::latency_is_enabled()){
			global_decomp = 1;
		//}
		// do decompression
		if (QuietMode != 1)
			fprintf(stderr, "Decompressing data with %d thread(s).\n", numCPU);

		ret = direct_decompress();
		if (ret != 0)
			errLevel = 1;               
	}
	else
	{
		// do compression code
		if (QuietMode != 1)
			fprintf(stderr, "Compressing data with %d threads(s).\n", numCPU);

		ret = direct_compress();
		if (ret != 0)
			errLevel = 1;
	} // else

	for (auto & element : IO_data_vec){

		if (element.OutputStdOut == 0)
		{
			// write store file meta data to output file
			if (writeFileMetaData(element.OutFilename) != 0)
				fprintf(stderr, "Bzip2: *ERROR: Could not write file meta data to [%s]!\n", element.InFilename);
		}

		// finished processing file
		element.sigInFilename = NULL;
		element.sigOutFilename = NULL;

		// remove input file unless requested not to by user
		if (keep != 1)
		{
			struct stat statbuf;
			if (element.OutputStdOut == 0)
			{
				// only remove input file if output file exists
				if (stat(element.OutFilename, &statbuf) == 0)
					remove(element.InFilename);
			}
			else
				remove(element.InFilename);
		}

		if (QuietMode != 1)
			fprintf(stderr, "-------------------------------------------\n");
	} /* for */

	/*	if (MemMutex != NULL)
		{
		pthread_mutex_destroy(MemMutex);
		delete MemMutex;
		MemMutex = NULL;
		}*/

	// get current time for end of benchmark
#ifndef WIN32
	gettimeofday(&tvStopTime, &tz);
#else
	GetSystemTime(&systemtime);
	SystemTimeToFileTime(&systemtime, (FILETIME *)&filetime);
	tvStopTime.tv_sec = filetime.QuadPart / 10000000;
	tvStopTime.tv_usec = (filetime.QuadPart - (LONGLONG)tvStopTime.tv_sec * 10000000) / 10;
#endif

#ifdef PBZIP_DEBUG
	fprintf(stderr, "\n Start Time: %ld + %ld\n", tvStartTime.tv_sec, tvStartTime.tv_usec);
	fprintf(stderr, " Stop Time : %ld + %ld\n", tvStopTime.tv_sec, tvStopTime.tv_usec);
#endif

	// convert time structure to real numbers
	timeStart = (double)tvStartTime.tv_sec + ((double)tvStartTime.tv_usec / 1000000);
	timeStop = (double)tvStopTime.tv_sec + ((double)tvStopTime.tv_usec / 1000000);
	timeCalc = timeStop - timeStart;
	if (QuietMode != 1)
		fprintf(stderr, "\n     Wall Clock: %f seconds\n", timeCalc);

	return errLevel;
}

} //end of namespace spb