# SPBench

![SPBench_logo](https://user-images.githubusercontent.com/11809226/121076611-fbef5300-c7ac-11eb-9213-5a30a284bec1.png)

SPBench is a new Framework for Benchmarking Stream Processing Applications.

The applications set comprises the following stream processing applications and will be more in the future:
 - Ferret (PARSEC)
 - Lane Detection
 - Bzip2
 - Person Recognition (Temporarily removed due to license constraint)

For each of them there are parallel versions implemented using the following parallel programming interfaces and will be more in the future:
 - Intel TBB
 - FastFlow
 - SPar
 - GrPPI (forthcoming)
 - Standard C++ Threads (forthcoming)

## How to install

The folowing packages are required to install SPBench:
 
- build-essential 
- cmake
- yasm
- python (tested on python3 and python2.7)

You can run the command bellow to install theses packages or ask a system admin.

`sudo apt-get install -y build-essential cmake yasm python3`

To install SPBench you can choose full installation (all applications):

`./spbench install`

Or choose a specific application:

`./spbench install -app <application name>`

Run `./spbench install -h` for more info.

For custom installations, see libs/README.md or ppis/README.md.

## How to run:

`./spbench [command] <arguments>`

or

`./spbench [command] <arguments>`

Try: `./spbench -h`
 
See INSTRUCTIONS.md for more info about all SPBench commands.

This is an ongoing project and you may face some bugs or other problems.
Feel free to open an issue or contact me for any problem regarding the SPBench.
adriano.garcia@edu.pucrs.br

