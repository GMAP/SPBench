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


def writeOperatorCpp(file_path, operator_id):
    """ !@brief Function for writing the source code of an example operator

    This function is used to write the source code of an example operator.

    @param file_path Path of the file.
    @param operator_id Name of the operator.
    """

    # ******************************************************************************************
    # create and write an example code for a operator
    # ******************************************************************************************

    # write the code for app_id.cpp
    op_cpp_file = open(file_path, 'a')

    op_cpp_file.write("\n#include <../include/" + operator_id + "_op.hpp>\n")
    op_cpp_file.write("\n")
    op_cpp_file.write("/**\n")
    op_cpp_file.write(" * @brief This function implements the " + operator_id + " operator.\n")
    op_cpp_file.write(" * \n")
    op_cpp_file.write(" * @param item (&item) \n")
    op_cpp_file.write(" */")
    op_cpp_file.write("\ninline void spb::" + operator_id + "::" + operator_id + "_op(spb::item_data &item){\n")
    op_cpp_file.write("\n")

    op_cpp_file.write("\t// All the code inside this function is only an usage example.\n")
    op_cpp_file.write("\t// Here we are just using the data item to do some basic and nonsense operations.\n")
    op_cpp_file.write("\t// You can delete it and write your own operator as you wish.\n")

    op_cpp_file.write("\tdouble some_double = 0.0;\n")
    op_cpp_file.write("\tbool some_bool = true;\n")
    op_cpp_file.write("\n")
    op_cpp_file.write("\titem.some_vector.push_back(std::to_string(some_double));\n")
    op_cpp_file.write("\n")
    op_cpp_file.write("\tfor (std::vector<std::string>::const_iterator iterator = item.some_vector.begin() ; iterator != item.some_vector.end() ; iterator++, item.some_unsigned_integer++){\n")
    op_cpp_file.write("\t\tsome_double = spb::Metrics::items_counter / some_external_variable;\n")
    op_cpp_file.write("\t\tif(some_bool){\n")
    op_cpp_file.write("\t\t\t\titem.some_unsigned_integer += some_external_variable;\n")
    op_cpp_file.write("\t\t}\n")
    op_cpp_file.write("\t}\n")
    op_cpp_file.write("\tif(item.some_unsigned_integer % 2 == 0){\n")
    op_cpp_file.write("\t\tstd::cout << spb::Metrics::items_counter <<  \" - This is a debug print. It will not be printed in execution time, unless you use the -debug flag.\" << std::endl;\n")
    op_cpp_file.write("\t}\n")
    op_cpp_file.write("\tusleep(100000);\n")
    op_cpp_file.write("\titem.some_vector.push_back(std::to_string(some_double));\n")
    op_cpp_file.write("}\n")

    op_cpp_file.close()
    # ******************************************************************************************



