## 
 ##############################################################################
 #  File  : appUtilHpp.py
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


def writeAppUtilsHpp(file_path, app_id, list_of_operators):
    """ !@brief Function for writing the header of the system-side headers of the application

    This function is used to write the header of the system-side headers of the application.

    @param file_path Path of the file.
    @param app_id Name of the application.
    @param list_of_operators List of operators.
    """
    prog_name = app_id + "_utils"
    # ******************************************************************************************
    # create and write the header of the system-side headers of the application
    # ******************************************************************************************

    # write the code for app_id.cpp
    app_hpp_utils_file = open(file_path, 'a')

    app_hpp_utils_file.write("\n#ifndef " + prog_name.upper() + "_HPP\n")
    app_hpp_utils_file.write("#define " + prog_name.upper() + "_HPP\n")
    app_hpp_utils_file.write("\n/** Includes **/\n")
    app_hpp_utils_file.write("#include <spbench.hpp>\n")
    app_hpp_utils_file.write("\nnamespace spb{\n")
    app_hpp_utils_file.write("\n#define NUMBER_OF_OPERATORS " + str(len(list_of_operators)) + "\n")
    app_hpp_utils_file.write("\nstruct item_data;\n")
    app_hpp_utils_file.write("class Item;\n")
    app_hpp_utils_file.write("class Source;\n")
    app_hpp_utils_file.write("class Sink;\n")
    app_hpp_utils_file.write("\nextern int some_external_variable;\n")
    app_hpp_utils_file.write("\nvoid init_bench(int argc, char* argv[]);\n")
    app_hpp_utils_file.write("void end_bench();\n")
    app_hpp_utils_file.write("\n/* Data from external input */\n")
    app_hpp_utils_file.write("struct workload_data {\n")
    app_hpp_utils_file.write("\tstd::string some_input_file;\n")
    app_hpp_utils_file.write("\tint some_integer;\n")
    app_hpp_utils_file.write("\tstd::string id;\n")
    app_hpp_utils_file.write("};\n")
    app_hpp_utils_file.write("extern workload_data input_data;\n")
    app_hpp_utils_file.write("\n/* All the data communicated between operators */\n")
    app_hpp_utils_file.write("struct item_data {\n")
    app_hpp_utils_file.write("\tstd::string *some_pointer;\n")
    app_hpp_utils_file.write("\tstd::string some_string;\n")
    app_hpp_utils_file.write("\tunsigned int some_unsigned_integer;\n")
    app_hpp_utils_file.write("\tstd::vector<std::string> some_vector;\n")
    app_hpp_utils_file.write("\tunsigned int index;\n")
    app_hpp_utils_file.write("\n")
    app_hpp_utils_file.write("\titem_data():\n")
    app_hpp_utils_file.write("\t\tsome_pointer(NULL),\n")
    app_hpp_utils_file.write("\t\tsome_unsigned_integer(0)\n")
    app_hpp_utils_file.write("\t{};\n")
    app_hpp_utils_file.write("\n")
    app_hpp_utils_file.write("\t~item_data(){\n")
    app_hpp_utils_file.write("\t\tsome_vector.clear();\n")
    app_hpp_utils_file.write("\t}\n")
    app_hpp_utils_file.write("};\n")
    app_hpp_utils_file.write("\n")
    app_hpp_utils_file.write("/* This class implements an Item */\n")
    app_hpp_utils_file.write("class Item : public Batch{\n")
    app_hpp_utils_file.write("public:\n")
    app_hpp_utils_file.write("\tstd::vector<item_data> item_batch;\n")
    app_hpp_utils_file.write("\n")
    app_hpp_utils_file.write("\tItem():Batch(NUMBER_OF_OPERATORS){};\n")
    app_hpp_utils_file.write("\n")
    app_hpp_utils_file.write("\t~Item(){}\n")
    app_hpp_utils_file.write("};\n")
    app_hpp_utils_file.write("\n")
    app_hpp_utils_file.write("class Source{\n")
    app_hpp_utils_file.write("public:\n")
    app_hpp_utils_file.write("\tstatic long source_item_timestamp;\n")
    app_hpp_utils_file.write("\tstatic bool op(Item &item);\n")
    app_hpp_utils_file.write("\tSource(){}\n")
    app_hpp_utils_file.write("\tvirtual ~Source(){}\n")
    app_hpp_utils_file.write("};\n")
    app_hpp_utils_file.write("\n")
    app_hpp_utils_file.write("class Sink{\n")
    app_hpp_utils_file.write("public:\n")
    app_hpp_utils_file.write("\tstatic void op(Item &item);\n")
    app_hpp_utils_file.write("\tSink(){}\n")
    app_hpp_utils_file.write("\tvirtual ~Sink(){}\n")
    app_hpp_utils_file.write("};\n")
    app_hpp_utils_file.write("\n")
    app_hpp_utils_file.write("} //end of namespace spb\n")
    app_hpp_utils_file.write("\n#endif\n")
   
    app_hpp_utils_file.close()
    # ******************************************************************************************
