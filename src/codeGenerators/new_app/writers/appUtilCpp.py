## 
 ##############################################################################
 #  File  : appUtilCpp.py
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


def writeAppUtilsCpp(spbench_path, file_path, app_id, operators_list):
    """ !@brief Function for writing the source code of the system-side code of the application

    This function is used to write the source code of the system-side code of the application.

    @param spbench_path Path of the SPBench root.
    @param file_path Path of the file.
    @param app_id Name of the application.
    @param operators_list List of operators.
    """

    # ******************************************************************************************
    # create and write the header of the system-side headers of the application
    # ******************************************************************************************

    # write the code for app_id.cpp
    app_util_cpp_file = open(file_path, 'a')

    app_util_cpp_file.write("\n#include <" + app_id + "_utils.hpp>\n")

    # Instead of writing manually the code, we can use a template to get specific lines of code and copy it to the new app
    template_cpp_util = spbench_path + "/sys/templates/simple_app_template.cpp"

    # check if template_cpp_util exists
    if not os.path.isfile(template_cpp_util):
        print("\n Error!! Missing template file \'" + template_cpp_util + "\'\n")
        sys.exit()

    # open the template file
    template_cpp_util_file = open(template_cpp_util, 'r')

    # copy template lines strating from line 2 and stoping when it found the string "Starting to set the operators name"
    lines = template_cpp_util_file.readlines()[1:]
    while "Starting to set the operators name" not in lines[0]:
        # write the lines in the new app
        app_util_cpp_file.writelines(lines[0])
        lines = lines[1:]

    # add source in the begining of the operators list and sink at the end
    operators_list.insert(0, "Source")
    operators_list.append("Sink")

    # add two spaces at the end of the longest operator name and fill the rest with spaces until they have the same size
    max_size = len(max(operators_list, key=len))
    for i in range(len(operators_list)):
        operators_list[i] = operators_list[i] + " " * (max_size - len(operators_list[i]) + 2)

    # write the set_operators_name() function
    set_op_func_header = """
/**
* @brief This method sets the operators name.
* 
* @param None
* @return None
*/
void set_operators_name(){
"""
    app_util_cpp_file.write(set_op_func_header)
    for operator in operators_list:
        app_util_cpp_file.write("\tSPBench::addOperatorName(\"" + operator + "\");\n")
    app_util_cpp_file.write("}\n")

    # copy the remaining lines from the template, from the line containing "End of setting the operators name" until the end of the file
    while "End of setting the operators name" not in lines[0]:
        lines = lines[1:]
    lines = lines[1:]
    app_util_cpp_file.writelines(lines)

    # close the files
    template_cpp_util_file.close()
    app_util_cpp_file.close()
    # ******************************************************************************************



