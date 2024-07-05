## 
 ##############################################################################
 #  File  : utils.py
 #
 #  Title : SPBench makefile generator
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

import os
import math
import logging

from .dict import *

#spbench_path_ = os.path.dirname(os.path.realpath(__file__)) + "/../../"

from sys import version_info 
python_3 = version_info[0]

class color:
    PURPLE = '\033[95m'
    CYAN = '\033[96m'
    DARKCYAN = '\033[36m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    END = '\033[0m'

# list of SPBench reserved words
reserved_words = {'all', 'bzip2', 'ferret', 'lane_detection', 'person_recognition', 'source', 'sink', 'spbench'}

# check if string is blank
def isNotBlank (my_string):
    return bool(my_string and my_string.strip())

def variance(data, ddof=0):
	n = len(data)
	mean = sum(data) / n
	return sum((x - mean) ** 2 for x in data) / (n - ddof)

def stdev(data):
	var = variance(data)
	std_dev = math.sqrt(var)
	return std_dev

def setup_logging():
    """Configure logging for the script."""
    logging.basicConfig(
        level=logging.INFO,  # Set logging level to INFO
        format='%(asctime)s - %(levelname)s - %(message)s',  # Define log message format
        datefmt='%Y-%m-%d %H:%M:%S',  # Define date format
        handlers=[logging.StreamHandler()]  # Output logs to console
    )    