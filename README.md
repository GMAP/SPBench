# SPBench - Easing the Complex

[![SPBench_logo](https://user-images.githubusercontent.com/11809226/121076611-fbef5300-c7ac-11eb-9213-5a30a284bec1.png)](https://spbench-doc.readthedocs.io/en/latest/)

[SPBench](https://spbench-doc.readthedocs.io/en/latest/) (Stream Processing Benchmark) is a framework for streamlining benchmarking of C++ stream processing. 
The main goal of SPBench is to enable users to easily create custom benchmarks from real-world stream processing applications and evaluate multiple PPIs.

<ins>**The SPbench documentation is available at https://spbench-doc.rtfd.io**</ins>

## How to cite

 - GARCIA, A. M.; SCHEPKE, C. ; GRIEBLER, D. J. ; FERNANDES, L. G. . SPBench: A Framework for Creating Benchmarks of Stream Processing Applications. COMPUTING, v. 1, p. 1, 2021, doi: 10.1007/s00607-021-01025-6. \[[link](https://doi.org/10.1007/s00607-021-01025-6)\]
 
 ```bibtex
@article{GARCIA:SPBench:22,
   title = {SPBench: a framework for creating benchmarks of stream processing applications},
   author = {Adriano Marques Garcia and Dalvan Griebler and Claudio Schepke and Luiz Gustavo Fernandes},
   url = {https://doi.org/10.1007/s00607-021-01025-6},
   doi = {10.1007/s00607-021-01025-6},
   year = {2022},
   date = {2022-01-01},
   journal = {Computing},
   volume = {In press},
   number = {In press},
   pages = {1-23},
   publisher = {Springer}
}
```

### Other papers using SPBench

 - A. M. Garcia, D. Griebler, C. Schepke and L. G. L. Fernandes, Micro-batch and data frequency for stream processing on multi-cores. The Journal of Supercomputing (2023), p. 1-39, doi: 10.1007/s11227-022-05024-y. \[[link](https://doi.org/10.1007/s11227-022-05024-y)\] 

 - A. M. Garcia et al., "A Latency, Throughput, and Programmability Perspective of GrPPI for Streaming on Multi-cores," 2023 31st Euromicro International Conference on Parallel, Distributed and Network-Based Processing (PDP), Naples, Italy, 2023, pp. 164-168, doi: 10.1109/PDP59025.2023.00033. \[[link](https://doi.org/10.1109/PDP59025.2023.00033)\]

 - A. M. Garcia, D. Griebler, L. G. L. Fernandes and C. Schepke, "Introducing a Stream Processing Framework for Assessing Parallel Programming Interfaces", 2021 29th Euromicro International Conference on Parallel, Distributed and Network-Based Processing (PDP), 2021, pp. 84-88, doi: 10.1109/PDP52278.2021.00021. \[[link](https://doi.org/10.1109/PDP52278.2021.00021)\] 

## The SPBench is intended for three main audiences:
- Users who want to run performance tests with the SPBench benchmarks. It implements different real-world stream processing applications using parallel programming interfaces (PPIs) and parallelism patterns and makes the primary metrics used in this domain available. The framework also enables users to create custom benchmarks with new PPIs.
- Researchers and developers who want to test and evaluate new technologies and solutions. The SPBench benchmarks are also highly parameterizable, and its API allows for easy and fast code reuse across all applications.
- Students and teachers who want to learn/teach stream parallelism. The benchmarks implemented with the SPBench API abstract the low-level code and expose to users only the stream core of each application (in a few lines of code). Therefore, it allows users to easily identify each operator and data dependencies and understand the stream flow. Then, users can build parallelism on top of this highly simplified code.

### The SPBench suite comprises the following stream processing applications and will be more in the future:
 - Ferret (PARSEC)
 - Lane Detection
 - Bzip2
 - Person Recognition (Face Recognizer)

### For each of them, there are parallel versions implemented using the following parallel programming interfaces, and will be more in the future:
 - Intel TBB
 - FastFlow
 - SPar
 - OpenMP
 - C++ Threads
 - GrPPI (backends: Intel TBB, FastFlow, OpenMP, and C++11 threads)

# SPBench Framework

One of the aspects that most differentiates SPBench from related solutions is how users interact with it.
The kernel of SPBench is its API that allows users to access simplified versions of the applications included in our suite. Based on the sequential versions, users can implement parallelism, create new custom benchmarks with different parallelism strategies, or even explore new parallel PPIs. 
Users can also configure and modify parameters, such as build dependencies and metrics, through a command-line interface (CLI). 
This CLI accesses a database containing all benchmarks from the SPBench suite.
Other secondary parameters can be tuned via the CLI with simple commands.
It allows users to entirely focus on writing and tuning the parallelism rather than spending time with the non-relevant low-level aspects of each application. The figure below illustrates the SPBench framework.

![SPBench_framework](https://gmap.pucrs.br/public_data/spbench/img/spbench_framework.png)

## How to install

The following packages are required to install SPBench:
 
- build-essential
- pkg-config
- cmake
- python (tested on python3)

You can run the command below to install these packages or ask a system admin.

`sudo apt-get install -y build-essential pkg-config cmake python3`

To install SPBench, you can run:

`./spbench install`

The command above will install the dependencies for all supported applications.
For installing dependencies for a single application, you can run:

`./spbench install -app <application name>`

Run `./spbench install -h` for more info.

For custom and manual installations, see libs/README.md or ppis/README.md.

**>> Attention <<**

It was tested with **GCC-9.4** compiler.
Some applications' dependency libraries, such as OpenCV, may not work with different versions or compilers.

## How to run:

`./spbench [command] <arguments>`

or

`python3 spbench [command] <arguments>`

Try: `./spbench -h` or `python3 spbench -h `
 
See the [documentation](https://spbench-doc.readthedocs.io/en/latest/) for more info about SPBench.

SPBench is an ongoing project, and you may face bugs or other problems.
Feel free to open an issue or contact me with any problem regarding the SPBench.
adriano1mg@gmail.com