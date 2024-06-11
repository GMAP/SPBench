## 
 ##############################################################################
 #  File  : download_inputs_option.py
 #
 #  Title : SPBench-CLI Download Inputs Option
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

from src.utils.utils import *
from src.utils.shell import *
from src.utils.usage import *
from src.utils.get_inputs import *

# download the inputs for the SPBench applications 
def download_inputs_func(spbench_path, args, skip = False):

    # Check if the chosen app exists
    if args.app_id:
        if (args.app_id not in getAppsList(spbench_path)) and (args.app_id != "all"):
            print("\n Application \'" + args.app_id + "\' not found!\n")
            sys.exit()
    else:
        args.app_id = "all"

    #overwrite_files=''
    #if args.force:
    #    overwrite_files = True

    if args.app_id == "all" and args.class_id == "all" and skip == False:
        print("\n Warning: No specific application selected.")
        print(" It will download all the inputs for all SPBench applications.")
        print("\n Aproximate size of all inputs: 1.3 GB.")
        if not askToProceed():
            sys.exit()

    print("\n >> It may take a while to finish <<\n")
    
    #get_inputs_file = spbench_path + "/inputs/get_inputs.py"
    if args.app_id == "lane_detection" or args.app_id == "all":
        get_inputs_func(spbench_path, "lane_detection", args.class_id, args.force)
        #runShellCmd("sh " + get_input_cmd)
    
    if args.app_id == "person_recognition" or args.app_id == "all":
        get_inputs_func(spbench_path, "person_recognition", "all", args.force)
        #runShellCmd("sh " + get_input_cmd)

    if args.app_id == "fraud_detection" or args.app_id == "all":
        get_inputs_func(spbench_path, "fraud_detection", "all", args.force)
        #runShellCmd("sh " + get_input_cmd)
    
    if args.app_id == "ferret" or args.app_id == "all":
        get_inputs_func(spbench_path, "ferret", args.class_id, args.force)
        #runShellCmd("sh " + get_input_cmd)
    
    if args.app_id == "bzip2" or args.app_id == "all":
        get_inputs_func(spbench_path, "bzip2", args.class_id, args.force)
        #runShellCmd(" " + get_input_cmd)

    sys.exit()
