## 
 ##############################################################################
 #  File  : operatorCpp.py
 #
 #  Title : SPBench application hpp generator
 #
 #  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 #
 #  Date  : November 20, 2023
 #
 #  Copyright (C) 2023 Adriano M. Garcia
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

from src.utils.shell import *
from src.utils.dict import *

def writeOpCpp(file_path, app_id, operators_list):
    """ !@brief Function for writing the hpp user-side code of the application

    This function is used to write the hpp user-side code of the application.

    @param file_path Path of the file.
    @param app_id Name of the application.
    @param operators_list List of operators.
    """

    # ******************************************************************************************
    # create and write a general header for the files
    # ******************************************************************************************

    # write the code for app_id.cpp
    hpp_file = open(file_path, 'a')

    hpp_file.write("\n#ifndef " + app_id.upper() + "_HPP\n")
    hpp_file.write("#define " + app_id.upper() + "_HPP\n")
    hpp_file.write("\n#include <" + app_id + "_utils.hpp>\n")
    hpp_file.write("\nnamespace spb{\n")
    for operator in operators_list:
        hpp_file.write("class " + operator + ";\n")

    for operator in operators_list:
        hpp_file.write("\n")
        hpp_file.write("class " + operator + "{\n")
        hpp_file.write("private:\n")
        hpp_file.write("\tstatic inline void " + operator + "_op(item_data &item);\n")
        hpp_file.write("public:\n")
        hpp_file.write("\tstatic void op(Item &item);\n")
        hpp_file.write("\t" + operator + "(Item &item){\n")
        hpp_file.write("\t\top(item);\n")
        hpp_file.write("\t}\n")
        hpp_file.write("\t" + operator + "(){};\n")
        hpp_file.write("\n")
        hpp_file.write("\tvirtual ~" + operator + "(){}\n")
        hpp_file.write("};\n")

    hpp_file.write("\n")
    hpp_file.write("} // end of namespace spb\n")
    hpp_file.write("#endif\n")

    hpp_file.close()
    # ******************************************************************************************



