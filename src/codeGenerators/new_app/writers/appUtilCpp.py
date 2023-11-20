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


def writeOperatorHpp(file_path, operator_id, app_id):
    """ !@brief Function for writing the header of a given operator

    This function is used to write the header of a given operator.

    @param file_path Path of the file.
    @param operator_id Name of the operator.
    """

    # ******************************************************************************************
    # create and write the header of a given operator
    # ******************************************************************************************

    # write the code for app_id.cpp
    op_hpp_file = open(file_path, 'a')

    op_hpp_file.write("\n#ifndef " + operator_id.upper() + "_OP_HPP\n")
    op_hpp_file.write("#define " + operator_id.upper() + "_OP_HPP\n")
    op_hpp_file.write("\n#include <" + app_id + ".hpp>\n")
    op_hpp_file.write("\nnamespace spb{\n")
    op_hpp_file.write("\n")

    op_hpp_file.write("/**\n")
    op_hpp_file.write(" * @brief This method implements the encapsulation of the " + operator_id + " operator.\n")
    op_hpp_file.write(" * \n")
    op_hpp_file.write(" * @param item (&item) \n")
    op_hpp_file.write(" */\n")

    op_hpp_file.write("void " + operator_id + "::op(Item &item){\n")
    op_hpp_file.write("\n")
    op_hpp_file.write("\tMetrics metrics;\n")
    op_hpp_file.write("\tvolatile unsigned long latency_op;\n")
    op_hpp_file.write("\tif(metrics.latency_is_enabled()){\n")
    op_hpp_file.write("\t\tlatency_op = current_time_usecs();\n")
    op_hpp_file.write("\t}\n")
    op_hpp_file.write("\tunsigned int num_item = 0;\n")
    op_hpp_file.write("\n")
    op_hpp_file.write("\twhile(num_item < item.batch_size){ //batch loop\n")
    op_hpp_file.write("\n")
    op_hpp_file.write("\t\t" + operator_id + "_op(item.item_batch[num_item]);\n")
    op_hpp_file.write("\n")
    op_hpp_file.write("\t\tnum_item++;\n")
    op_hpp_file.write("\t}\n")
    op_hpp_file.write("\t\n")
    op_hpp_file.write("\tif(metrics.latency_is_enabled()){\n")
    op_hpp_file.write("\t\titem.latency_op.push_back(current_time_usecs() - latency_op);\n")
    op_hpp_file.write("\t}\n")
    op_hpp_file.write("}\n")
    op_hpp_file.write("\n")
    op_hpp_file.write("} // end of namespace spb\n")

    op_hpp_file.close()
    # ******************************************************************************************



