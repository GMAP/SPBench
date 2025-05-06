## 
 ##############################################################################
 #  File  : usage.py
 #
 #  Title : Usage script for SPBench
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

import math
from .utils import *

# check if string is blank
def isNotBlank (my_string):
    return bool(my_string and my_string.strip())

def askToProceed():
    # user input support for python 2 and 3
    if(python_3 == 3):
        answer = input("\n Do you want to proceed? [yes/no]\n ")
    else:
        answer = raw_input("\n Do you want to proceed? [yes/no]\n ")

    if(answer.lower() not in ["y","yes"]): # delete the old benchmark
        print(" Operation canceled!\n")
        return False

    return True

def variance(data, ddof=0):
	n = len(data)
	mean = sum(data) / n
	return sum((x - mean) ** 2 for x in data) / (n - ddof)

def stdev(data):
	var = variance(data)
	std_dev = math.sqrt(var)
	return std_dev