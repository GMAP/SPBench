## 
 ##############################################################################
 #  File  : install_option.py
 #
 #  Title : SPBench-CLI Intall Option
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

from src.utils.shell import *
from src.download_inputs_option import *
from src.utils.install_libs import *
from src.utils.utils import *

def install_func(spbench_path, args):

    setup_logging()

    print(" ---------------------------------------------------------------")
    print(color.BOLD + "                         >> ATTENTION <<                         " + color.END)
    print(" ---------------------------------------------------------------\n")
    print(" Before proceeding with the installation you\n need to ensure that you have the following\n packages installed on your system:")
    print("    - build-essential")
    print("    - cmake")
    print("    - python\n")
    print(" To install them on a Ubuntu system, for example, you can run:\n")
    print(" sudo apt install -y build-essential cmake python3.\n")
    print(" ---------------------------------------------------------------\n")

    YASM_VERSION = "YASM-v1.3.0"
    FFMPEG_VERSION = "FFMPEG-v3.4.8"
    OPENCV_VERSION = "OpenCV-v2.4.13.6"
    BZLIB_VERSION = "BZLIB-v1.0.8"
    GSL_VERSION = "GSL-v2.6"
    JPEG_VERSION = "LIBJPEG-TURBO-v1.5.2"

    print(" You are installing dependencies for the following\n application:")
    print("\n -> " + args.app_id + "\n")
    if(args.app_id == "lane_detection") or (args.app_id == "person_recognition"):
        print(f" This will install {YASM_VERSION}, {FFMPEG_VERSION}, and {OPENCV_VERSION}.\n")
        print(" >> It may take several minutes. <<")
    elif(args.app_id == "ferret"):
        print(f" This will install {YASM_VERSION}, {GSL_VERSION}, and {JPEG_VERSION}.\n")
        print(" It may take a few minutes.")
    elif(args.app_id == "bzip2"):
        print(f" This will install {BZLIB_VERSION}.\n")
        print(" It may take a few moments.")
    else:
        print(f" This will install {BZLIB_VERSION}, {YASM_VERSION}, {FFMPEG_VERSION}, {OPENCV_VERSION}, {GSL_VERSION}, and {JPEG_VERSION}.\n")
        print(" >> It may take several minutes. <<")
        
    # user input support for python 2 and 3
    if(python_3 == 3):
        answer = input(color.BOLD + "\n Do you want to proceed with the installation? [yes/no]\n " + color.END)
    else:
        answer = raw_input(color.BOLD + "\n Do you want to proceed with the installation? [yes/no]\n " + color.END)

    if(answer.lower() not in ["y","yes"]):
        print("Installation canceled!\n")
        sys.exit()

    print(" ---------------------------------------------------------------\n")

    print(" For running the benchmarks you can use the inputs provided by SPBench.")
    print(" Otherwise, you can use your own inputs.")

    print(" You can download them now or later, using the command \'./spbench download-inputs\'")
    print("\n Aproximated size of all inputs: 1.3 GB.")

    download_inputs_flag = True
    # user input support for python 2 and 3
    if(python_3 == 3):
        answer = input(color.BOLD + "\n Do you want to download all benchmark inputs now? [yes/no]\n " + color.END)
    else:
        answer = raw_input(color.BOLD + "\n Do you want to download all benchmark inputs now? [yes/no]\n " + color.END)

    if(answer.lower() not in ["y","yes"]):
        download_inputs_flag = False

    print(" ---------------------------------------------------------------\n")

    print(" For running the parallel benchmarks you must also install the\n parallelism libraries (FastFlow, TBB, GrPPI, SPar, WindFlow).")
    print(f"\n You can install them now or later, running the script in\n \'{spbench_path}/ppis/install_ppis.sh\'")

     # user input support for python 2 and 3
    if(python_3 == 3):
        answer = input(color.BOLD + "\n Do you want to download and install the parallelism libraries now? [yes/no]\n " + color.END)
    else:
        answer = raw_input(color.BOLD + "\n Do you want to download and install the parallelism libraries now? [yes/no]\n " + color.END)

    if(answer.lower() in ["y","yes"]):
        runShellCmd('sh ' + spbench_path + '/ppis/install_ppis.sh')
    
    #runShellCmd('sh ' + spbench_path + '/libs/install_libs.sh ' + args.app_id)
    install_libraries(spbench_path, args.app_id)

    print(" ---------------------------------------------------------------")
    print(color.BOLD + "                         >> IMPORTANT <<                         " + color.END)
    print(" ---------------------------------------------------------------\n")
    print(" Now you must run \'source setup_vars.sh\'\n to load the libraries.\n")
    print(" Obs: You must load the libraries before using\n SPBench every time you start a new session!\n")
    
    if download_inputs_flag:
        args.app_id = "all"
        args.class_id = "all"
        args.force = True
        download_inputs_func(spbench_path, args, True)

    sys.exit()
