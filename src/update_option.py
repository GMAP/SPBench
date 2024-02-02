## 
 ##############################################################################
 #  File  : register_option.py
 #
 #  Title : SPBench commands manager
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

from src.utils.dict import *
from src.codeGenerators.make_gen import *

def update_func(spbench_path, args):

    # get list of selected benchmarks to run
    benchmarks_to_run = registryDicToList(filterRegistry(getBenchRegistry(spbench_path), args))

    for benchmark in benchmarks_to_run:
        #app_id = benchmark["app_id"]
        #ppi_id = benchmark["ppi_id"]
        #bench_id = benchmark["bench_id"]

        print(" Updating compiling configurations for: " + benchmark["bench_id"])

        make_gen(spbench_path, benchmark)
