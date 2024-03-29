/** 
 * ************************************************************************  
 *  File  : bzip2_utils_ns.hpp
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

#ifndef BZIP2_U
#define BZIP2_U

#include <vector>
#include <string>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <utime.h>
#include <bzlib.h>
#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#else
#include <windows.h>
#include <io.h>
#endif
#ifdef __APPLE__
#include <sys/sysctl.h>
#endif
#ifdef __sun
#include <sys/loadavg.h>
#endif
#ifndef  __BORLANDC__
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#else
#define strncasecmp(x,y,z) strncmpi(x,y,z)
#endif

#ifdef ENABLE_UPL
#include <upl.h>
#include <iostream> 
#endif

	/* ----------------------------------------------------- */


	// uncomment for debug output
	//#define PBZIP_DEBUG

	// uncomment to disable load average code (may be required for some platforms)
	//#define PBZIP_NO_LOADAVG

	// detect systems that are known not to support load average code
#if defined (WIN32) || defined (__CYGWIN32__) || defined (__MINGW32__) || defined (__BORLANDC__) || defined (__hpux) || defined (__osf__) || defined(__UCLIBC__)
#define PBZIP_NO_LOADAVG
#endif

#ifdef WIN32
#define PATH_SEP	'\\'
#define usleep(x) Sleep(x/1000)
#define LOW_DWORD(x)  ((*(LARGE_INTEGER *)&x).LowPart)
#define HIGH_DWORD(x) ((*(LARGE_INTEGER *)&x).HighPart)
#ifndef _TIMEVAL_DEFINED /* also in winsock[2].h */
#define _TIMEVAL_DEFINED
	struct timeval {
		long tv_sec;
		long tv_usec;
	};
#endif
#else
#define PATH_SEP	'/'
#endif

#ifndef WIN32
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define OFF_T		off_t
#else
#define	FILE_MODE	(S_IRUSR | S_IWUSR )
#define OFF_T		__int64
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#define NUMBER_OF_OPERATORS 3

#include <spbench.hpp>

void compress();
void decompress();

namespace spb{

class Item;

class Source;
class Sink;

//
// GLOBALS
//

extern int BWTblockSize;
extern int blockSize;
extern int blockNum;
extern int Verbosity;

extern unsigned int global_decomp; //0 for compress and 1 for decompress

struct input_data_inmemory;
struct IO_data_struct;
struct item_data;
struct bz2BlockListing;

extern std::vector<IO_data_struct> IO_data_vec;

int bzip2_main(int argc, char* argv[]);
void end_bench();

int direct_compress(char *);
int direct_decompress(char *);

/*
 *********************************************************
 */

struct bz2BlockListing{
	OFF_T dataStart;
	OFF_T dataSize;
};


struct input_data_inmemory{ 
	char *block;
	OFF_T buffSize;
};

struct IO_data_struct {
	OFF_T fileSize;
	int hInfile;
	unsigned int outFileSize;
	int OutputStdOut;
	char OutFilename[2048];
	int hOutfile;
	std::string id;
	std::vector<input_data_inmemory> MemData;

	struct stat fileMetaData;
	char *InFilename;
	char *sigInFilename;
	char *sigOutFilename;

	IO_data_struct():
		fileSize(0),
		hInfile(-1),
		outFileSize(0),
		OutputStdOut(0),
		InFilename(NULL),
		sigInFilename(NULL),
		sigOutFilename(NULL)
	{}
};

struct item_data{
	int index;
	OFF_T buffSize;
	char *FileData;
	char *CompDecompData;
	unsigned int sourceId;

	item_data():
		index(0),
		buffSize(0),
		FileData(NULL),
		CompDecompData(NULL)
	{}
};

class Item : public Batch, public NsItem{
public:
	std::vector<item_data> item_batch;

	Item():Batch(NUMBER_OF_OPERATORS){};

	~Item(){}
};



class Source : public SuperSource{
	private:

		unsigned int sourceQueueSize;
		std::vector <bz2BlockListing> bz2BlockList;

		bool source_comp();
		void init_bench();
		void source_decomp();
		void printStatus();

		void checkInput(){
			if(IO_data_vec.size() < sourceObjCounter){
				std::cout << " Error!! You must provide an input for each source." << std::endl;
				std::cout << " - Created sources: " << sourceObjCounter << std::endl;
				std::cout << " - Given inputs: " << IO_data_vec.size() << std::endl;
				std::cout << std::endl;
				exit(0);
			}
			return;
		}

		void tryToJoin(){ // If thread Object is Joinable then Join that thread.
			if (source_thread.joinable()){
				source_thread.join();
			}
		}

	public:
		std::vector<item_metrics_data> sourceLatencyVector;
		concurrent::queue::blocking_queue<Item> sourceQueue;

		Source() : SuperSource(){
			setBatchSize(SPBench::getBatchSize());
			setBatchInterval(SPBench::getBatchInterval());
			setQueueMaxSize(0);
			setFrequency(SPBench::getFrequency());
			setSourceId(sourceObjCounter);
			setSourceDepleted(false);
			setRunningState(false);
			sourceObjCounter++;
		}

		Source(int batchSize, float batchInterval, int queueSize, long frequency) : SuperSource(){
			sourceDepleted = false;
			sourceId = sourceObjCounter;
			sourceObjCounter++;
			checkInput();
			setFrequency(frequency);
			setQueueMaxSize(queueSize);
			setBatchSize(batchSize);
			setSourceName(IO_data_vec[sourceId].InFilename);
			source_metrics = init_metrics();
			source_metrics.sourceId = sourceId;
			source_metrics.source_name = getSourceName();
			metrics_vec.push_back(source_metrics);
			if(global_decomp == 0){
				source_thread = std::thread(&Source::source_comp, this);
			} else {
				source_thread = std::thread(&Source::source_decomp, this);
			}
			isRunning = true;
			printStatus();
		}

		~Source(){}
		
		void init(){
			if(isRunning){
				std::cout << "Failed to use init(), this source is already running!" << std::endl;
				return;
			}
			checkInput();
			setSourceName(IO_data_vec[sourceId].InFilename);
			source_metrics = init_metrics();
			source_metrics.sourceId = sourceId;
			source_metrics.source_name = getSourceName();
			metrics_vec.push_back(source_metrics);
			if(global_decomp == 0){
				source_thread = std::thread(&Source::source_comp, this);
			} else {
				source_thread = std::thread(&Source::source_decomp, this);
			}
			isRunning = true;
			printStatus();
		}

		Item getItem(){
			
			Item item;
			
			item.sourceId = sourceId;
			
			if(depleted() || !isRunning) { return item;}

			item = sourceQueue.dequeue();
			item.sourceId = sourceId;

			if (sourceQueue.front().isLastItem()){
				std::cout << "\n -> Source depleted: " << getSourceName() << std::endl;
				sourceDepleted = true;
				tryToJoin();
			}
			return item;
		}

		void setSourceName(std::string sourceName){
			if(global_decomp == 0){
				this->sourceName = remove_extension(base_name<std::string>(sourceName));
			} else {
				this->sourceName = remove_extension(remove_extension(base_name<std::string>(sourceName)));
			}
			return;
		}

		void setQueueMaxSize(unsigned int newQueueSize){
			
			if(newQueueSize < 0){
				std::cout << " Queue size must be 0 (unlimited) or higher.\n";
				return;
			}

			if(newQueueSize < sourceQueue.size()){
				std::cout << " You cannot set a max. queue size lower than the number of current items in the queue.\n";
				std::cout << " Number of items in the queue: " << sourceQueue.size() << std::endl;
				return;
			}

			sourceQueueSize = newQueueSize;
			sourceQueue.setCapacity(sourceQueueSize);
		}
};

class Sink{
private:
	static void sink_c(Item &item);
	static void sink_d(Item &item);
public:
	//static void op(Item &item);
	Sink(){}
	virtual ~Sink(){}

	static void op(Item &item){
		if(global_decomp == 1){sink_d(item);}
		else{sink_c(item);}
	}
};

} //end of namespace spb

#endif