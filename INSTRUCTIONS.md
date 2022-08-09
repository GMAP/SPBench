# Instructions

Here we give some how-to-use instructions and tips.

Notice that these are basic, simplified instructions. For more details and the complete documentation, go to: [https://spbench-doc.rtfd.io](https://spbench-doc.rtfd.io)

## Command-Line Interface:

The framework works with a command-line interface (CLI).

The CLI offers some commands. You can check them running:

`./spbench --help`

For each command there is also a "help" guide. You can access it using:

`./spbench [command] -h`

## Basic concepts:

 - **Benchmark**: In this context, it means any application version registered in SPBench. Benchmarks can be the sequential applications, the parallel versions pre-available within SPBench, or any new custom implementations registered latter by users.
 - **PPI**: It stands for Parallel Programming Interface, which means any library, framework, language, etc. that leverages parallelism.

## Framework commands:

The table bellow shows all commands that can be used through the CLI
| Command | Description | Subcommands |
|---|---|---|
| **install** | It installs the dependencies for the SPBench benchmarks. | `-app <some_application>` |
| **register** | It automatically sets the paths and generates new makefiles for all benchmarks. | None |
| **new** | It allows for creating a new benchmark based on a SPBench application.<br /> This command will create a fresh copy of the selected application. | `-name <new_benchmark_id>`(any name)<br /> `-app <some_SPBench_application>`<br /> `-ppi <some_ppi>` (any name)<br /> `-replicate <other_benchmark_id>`(Optional) |
| **edit** | It opens a choosen benchmark for code editing. | `-bench <benchmark_id>`<br /> `-editor <editor_name>`(Optional)(Default: nano) |
| **configure** | It allows users to add compiling dependencies for a benchmark.<br /> It opens a JSON configuration file for you to write the required dependencies. | `-bench <benchmark_id>`<br /> `-editor <editor_name>`(Optional)(Default: nano) |
| **compile** | It compiles a selected benchmark. | `-bench <benchmark_id>` |
| **exec** | It runs a selected benchmark. | `-bench <benchmark_id>`<br /> `-input <input_id>`<br /> `-batch <batch_size>`(optional)<br /> `-frequency <item_delay>`(microsecond)(optional)<br /> `-nthreads <number_of_threads>`(optional)<br /> `-in-memory`(optional)<br /> `-latency`(optional)<br /> `-throughput`(optional)<br /> `-resource_usage`(optional)<br /> `-monitor <time_interval>`(millisecond)(optional)<br /> `-latency_monitor`(optional)<br /> `-user_arg <user_custom_argument>`(optional) |
| **list** | This command lists all SPBench benchmarks. Users' new benchmark versions are added to this list. | None |
| **delete** | It completely deletes the given benchmark. | `-bench <benchmark_id>` |
| **new_input** | It allows for adding an alias for a custom input. | `-input_id <new_input_id>`<br /> `-app <SPBench_application>`<br /> `-input <"input info/string">` |
| **list_input** | It lists all inputs registered for a given application and their respective alias.<br /> (Attention: You can only use pre-registered alias after downloading the respective inputs.<br /> See the section about inputs below). | `-app <SPBench_application>`(optional) |



## How to use

In this framework you can simply run the benchmarks that are already provided or implement your own specific benchmark.

To run a given benchmark version, you can follow these steps:
 - Run: `./spbench install` (you can use `-app <some application>` to install only a specific application)
 - Run: `./spbench list` (this shows the list of available benchmarks)
 - Run: `./spbench compile -benchmark <benchmark_id>`
 - Run: `./spbench exec -benchmark <benchmark_id> -input <input_id> ... <optional_args>`

To implement your own benchmark version, you must run **new -> edit -> configure**

## Inputs

In the **exec** step you must choose a given input for the benchmark.

To automatically download them you can run:

`./spbench download-inputs`

You can use your own input or choose a given one.

You can also use an alias as short option.
These alias are linked to the inputs provided by SPBench and you can see them running:
`./spbench list_inputs`

SPBench also allows you to create an alias for your custom inputs.
`./spbench new_input -id <input_alias> -app <spbench_application> -input "<full_path_to_your_new_input>"`

## How to run example:

- `./spbench exec -benchmark [benchmark_name] -input [small, medium, large]`

## Performance metrics

With SPBench you can evaluate four performance metrics in different ways.
    - Latency
    - Throughput
    - CPU usage (%)
    - Memory usage (KB)

There are some OPTIONAL arguments you can select when running the 'exec' command:

## For metrics:

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

## Other optional management arguments

All of these are also OPTIONAL arguments that you can use within the 'exec' command.

- `-in-memory`

    It runs the application in-memory, which means that all the input is first loaded into the memory before start processing it. The opposite is true for the writing phase, where the result stays in memory and is only writen to the disk after all the input is proccessed.

- `-nthreads <number_of_threads>`

    Here you can define a number of threads to run the parallel versions. To be more preciselly, the degree of parallelism of the parallel stages (Obs.: some PPIs can create more threads than parallel stages). It shows up as a global variable that can be used inside the parallel versions.

- `-batch <batch_size>`

    Here you can change the size of the batches, so each item can carry more data. By default, the size of each batch is 1.

- `-frequency <time-delay_per_item_in_us>`

    Here you can set the frequency in which the sources of the benchmarks will produce new items. To increase or decrese the frequency, you can decrease or increase the time-delay between items generation. You must insert a time-delay in microseconds (e.g. 500000 = 500 ms).

- `-d`
    
    This a specific argument for bzip2. You can use it to run this application in decompress mode. Observe that for de decompression mode it is required a compressed file as input (e.g. my_compressed_file.tar.bz2).

## Important

You must run `source setup_vars.sh` to load the libraries.

You must load the libraries before using SPBench every time you star a new session!

To make FastFlow recognize and map the cores of the processor go into "/libs/SPar/libraries/ff/" and "/libs/fastflow/ff/" and run the "mapping_string.sh" script.


