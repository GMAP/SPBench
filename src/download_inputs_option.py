## 
 ##############################################################################
 #  File  : download_inputs_option.py
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

import sys

from src.utils import *

# download the inputs for the SPBench applications 
def download_inputs_func(spbench_path, args):

    # Check if the chosen app exists
    if args.app_id:
        if (args.app_id not in apps_list) and (args.app_id != "all"):
            print("\n Application \'" + args.app_id + "\' not found!\n")
            sys.exit()
    else:
        args.app_id = "all"

    overwrite_files=''
    if args.force:
        overwrite_files = "force"

    if args.app_id == "all" and args.class_id == "all":
        print("\n Warning: No specific application selected.")
        print(" It will download all the inputs for all SPBench applications.")
        print("\n Aproximate size of all inputs: 800 MB.")
        if not askToProceed():
            sys.exit()

    if args.app_id == "lane_detection" or args.app_id == "all":
        get_input_cmd = spbench_path + "/inputs/get_inputs.sh " + "lane_detection " + "trash " + overwrite_files
        os.system("bash " + get_input_cmd)
    
    if args.app_id == "person_recognition" or args.app_id == "all":
        get_input_cmd = spbench_path + "/inputs/get_inputs.sh " + "person_recognition " + "trash " + overwrite_files
        os.system("bash " + get_input_cmd)
    
    if args.app_id == "ferret" or args.app_id == "all":
        get_input_cmd = spbench_path + "/inputs/get_inputs.sh " + "ferret " + "trash " + overwrite_files
        os.system("bash " + get_input_cmd)
    
    if args.app_id == "bzip2" or args.app_id == "all":
        available_classes = ["small", "medium", "large", "all"]
        if not args.class_id:
            args.class_id = "all"
        else:
            if args.class_id not in available_classes:
                print("\n Error: invalid workload class!\n")
                print(" You can select from [small, medium, large, all] or leave this option empty.\n")

        get_input_cmd = spbench_path + "/inputs/get_inputs.sh " + "bzip2 " + args.class_id + " " + overwrite_files
        os.system("bash " + get_input_cmd)

    sys.exit()
