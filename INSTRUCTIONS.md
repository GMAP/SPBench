# Instructions

Here we give some how-to-use instructions and tips.

## Command-Line Interface: 

The framework works with a command-line interface (CLI).

The CLI offers some commands. You can check them running:

`./spbench --help`

For each command there is also a "help" guide. You can access it using:

`./spbench [command] -h`

## Framework commands:

 - **install**: It install the dependencies for the SPBench applications and parallel implementations.
 - **register**: It automatically generates makefiles for all benchmark versions available within the framework (You must install the dependencies first).
 - **new**: You use it to create new version of a given application from the framework. This command will create a fresh copy of the selected application for you to work on. See the 'help' option for this command to check the required arguments.
 - **edit**: You can use this comand to edit a given application version. See the 'help' option for this command to check the required arguments.
 - **configure**: You can use this command to configure the compiling dependencies of an application version. It opens a JSON configuration file for you to write the required dependencies. See the 'help' option for this command to check the required arguments.
 - **compile**: Use this command to compile a given version. See the 'help' option for this command to check the required arguments.
 - **exec**: Use this command to run an application version. See the 'help' option for this command to check the required arguments.
 - **list**: This command lists all applications versions of the framework. Your new versions will be added to this list.
 - **delete**: It completely deletes a given application version.
 - **new_input**: It allows you to add an alias for a custom input. See the 'help' option for this command to check the required arguments.
 - **list_inputs**: List all inputs registered for a given application and their respective alias. (Attention: You can only use these alias after downloading the respective inputs. See the section about inputs below).

## How to use

In this framework you can simply run the benchmarks that are already provided or implement your own specific benchmark.

To run a given benchmark version, you can follow these steps:
 - Run: `./spbench install` (you can use `-app <some application>` to install only a specific application)
 - Run: `./spbench register`
 - Run: `./spbench list` (this shows the list of available benchmarks)
 - Run: `./spbench compile -version [version_name]`
 - Run: `./spbench exec -version [version_name] -input [some_input] [other_args]`

To implement your own benchmark version, you must run **new -> edit -> configure**

## Inputs

In the **exec** step you must choose a given input for the benchmark.

These inputs can be found and downloaded in the inputs/ directory.

You can run the 'get_inputs.sh' script to automatically download them.

`cd inputs/ && bash get_inputs.sh`

You can use your own input or choose a given one.

You can also use an alias as short option.
These alias are linked to the inputs provided by SPBench and you can see them running:
`./spbench list_inputs`

SPBench also allows you to create an alias for your custom inputs.
`./spbench new_input -input_id <input_alias> -app <spbench_application> -input "<full_path_to_your_new_input>"`

## How to run example:

- `./spbench exec -version [version_name] -input [small, medium, large]`

## Performance metrics

With SPBench you can evaluate four performance metrics in different ways.
    - Latency
    - Throughput
    - CPU usage (%)
    - Memory usage (KB)

There are four OPTIONAL arguments you can select when running the 'exec' command:

- `-monitor <time interval>`

    It monitors latency, throughput, CPU and memory usage. Here you can indicate a monitoring time interval in milliseconds (e.g. 250.Optional). It will generate a log file inside a log folder (spbench/log/). This file will contain the results got for each metric at each time interval.

- `-latency`

    It prints the average latency results for each application operator on the screen after the execution.

- `-throughput`

    It prints the average throughput results after the execution.

- `-latency_monitor`

    It monitors the latency per stage and per item and generates a log file inside the log folder.

- `-resource_usage`

    It prints the global memory and CPU usage for the selected application.

## Other configuration arguments

All of these are also OPTIONAL arguments that you can use within the 'exec' command.

- `-in_memory`

    It runs the application in-memory, which means that all the input is first loaded into the memory before start processing it. The opposite is true for the writing phase, where the result stays in memory and is only writen to the disk after all the input is proccessed.

- `-nthreads <number_of_threads>`

    Here you can define a number of threads to run the parallel versions. To be more preciselly, the degree of parallelism of the parallel stages (Obs.: some PPIs can create more threads than parallel stages). It shows up as a global variable that can be used inside the parallel versions.

- `-batch <batch_size>`

    Here you can change the size of the batches, so each item can carry more data. By default, the size of each batch is 1.
    
- `-frequency <time_in_microseconds>`

    Here you can change the frequency of itens generated by source operator. The minimum frequency is limited by the memory access speed in your system.

- `-d`
    
    This a specific argument for bzip2. You can use it to run this application in decompress mode. Observe that for de decompression mode it is required a compressed file as input (e.g. my_compressed_file.tar.bz2).

## Global attributes

SPBench implements some global variables that can be used and modified by users at execution time in custom implementations. Some useful are:

- nthreads
- batch_size
- items_reading_frequency

## Important

You must run `source setup_vars.sh` to load the libraries.

You must load the libraries before using SPBench every time you star a new session!

To make FastFlow recognize and map the cores of the processor go into "/libs/SPar/libraries/ff/" and "/libs/fastflow/ff/" and run the "mapping_string.sh" script.


