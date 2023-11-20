## 
 ##############################################################################
 #  File  : appCpp.py
 #
 #  Title : SPBench main function generator
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

def writeAppCpp(file_path, app_id, operators_list):
    """ !@brief Function for writing the main code of the application

    This function is used to write the main code of the application.

    @param file_path Path of the file.
    @param app_id Name of the application.
    @param operators_list List of operators.
    """

    # ******************************************************************************************
    # create and write a general header for the files
    # ******************************************************************************************

    # write the code for app_id.cpp
    app_cpp_file = open(file_path, 'a')

    app_cpp_file.write("\n#include <" + app_id + ".hpp>\n")
    app_cpp_file.write("\nint main (int argc, char* argv[]){\n")
    app_cpp_file.write("\tspb::init_bench(argc, argv); // Initializations\n")
    app_cpp_file.write("\tspb::Metrics::init();\n")
    app_cpp_file.write("\twhile(1){\n")
    app_cpp_file.write("\t\tspb::Item item;\n")
    app_cpp_file.write("\t\tif(!spb::Source::op(item)) break;\n")
    for operator in operators_list:
        app_cpp_file.write("\t\tspb::" + operator + "::op(item);\n")
    app_cpp_file.write("\t\tspb::Sink::op(item);\n")
    app_cpp_file.write("\t}\n")
    app_cpp_file.write("\tspb::Metrics::stop();\n")
    app_cpp_file.write("\tspb::end_bench();\n")
    app_cpp_file.write("\treturn 0;\n")
    app_cpp_file.write("}\n")

    app_cpp_file.close()
    # ******************************************************************************************



