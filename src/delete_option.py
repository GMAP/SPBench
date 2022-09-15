## 
 ##############################################################################
 #  File  : delete_option.py
 #
 #  Title : SPBench-CLI Delete Benchmark Option
 #
 #  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 #
 #  Date  : July 06, 2021
 #
 #  Copyright (C) 2021 Adriano M. Garcia
 # 
 #  This program is free software: you can redistribute it and/or modify
 #  it under the terms of the GNU General Public License as published by
 #  the Free Software Foundation, either version 3 of the License, or
 #  (at your option) any later version.
 #
 #  This program is distributed in the hope that it will be useful,
 #  but WITHOUT ANY WARRANTY; without even the implied warranty of
 #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 #  GNU General Public License for more details.
 #
 #  You should have received a copy of the GNU General Public License
 #  along with this program. If not, see <https://www.gnu.org/licenses/>.
 # 
 ##############################################################################
##

import sys
import os

from src.utils import *

#delete a single registry entry
def delete_reg_func(spbench_path, args):

    # TODO - Delete only binary and outputs (clean)

    if args.benchmark_id == 'all':
        print("\n Error!! You cannot delete all benchmarks at once.\n Try again using a single benchmark ID.\n")
        sys.exit()

    # get the selected benchmark (it is a list)
    selected_benchmark = registryDicToList(filterRegistry(getBenchRegistry(spbench_path), args))

    app_id = selected_benchmark[0]["app_id"]
    ppi_id = selected_benchmark[0]["ppi_id"]
    bench_id = selected_benchmark[0]["bench_id"]

    print(" ---------------------------------------------------------------")
    print(color.BOLD + "                            >> WARNING <<                         " + color.END)
    print(" ---------------------------------------------------------------")
    print("\n This option will delete all files related to this benchmark.")
    print("\n Please, double check the benchmark to delete bellow before proceeding.")
    print("\n Benchmark info:")
    print(" - Bench ID: " + bench_id)
    print(" - Base app: " + app_id)
    print(" - PPI used: " + ppi_id)
    if not askToProceed():
        sys.exit()

    delete_benchmark(spbench_path, args)

    print("\n -> " + bench_id + " deleted!\n")

def delete_benchmark(spbench_path, args):

    # get the selected benchmark (it is a list)
    selected_benchmark = registryDicToList(filterRegistry(getBenchRegistry(spbench_path), args))

    app_id = selected_benchmark[0]["app_id"]
    ppi_id = selected_benchmark[0]["ppi_id"]
    bench_id = selected_benchmark[0]["bench_id"]

    # remove the respective directory and files
    dir_to_remove = spbench_path + "/benchmarks/" + app_id + "/" + ppi_id + "/" + bench_id + "/"

    if dirExists(dir_to_remove):
        runShellCmd("rm -r " + dir_to_remove)

    registry_dic = getBenchRegistry(spbench_path)

    # remove benchmark from registry
    del registry_dic[app_id][ppi_id][bench_id]

    # if there is no other benchmark related to this PPI, remove also the PPI directory
    ppi_dir = spbench_path + "/benchmarks/" + app_id + "/" + ppi_id
    if dirExists(ppi_dir):
        directory = os.listdir(ppi_dir) 
        if len(directory) == 0: 
            runShellCmd("rm -r " + ppi_dir)
            # remove also the PPI key from registry
            del registry_dic[app_id][ppi_id]

    # remove the binary
    bin_path = spbench_path + "/bin/" + app_id + "/" + ppi_id + "/" + bench_id
    if os.path.exists(bin_path):
        runShellCmd("rm " + bin_path)

    # update the registry
    writeDicToBenchRegistry(spbench_path, registry_dic)



