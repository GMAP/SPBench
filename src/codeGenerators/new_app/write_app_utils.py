## 
 ##############################################################################
 #  File  : write_apps_utils.py
 #
 #  Title : SPBench app utils writer
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

from datetime import date

from src.utils.shell import *
from src.utils.dict import *
import src.codeGenerators.new_app.writers.appUtilCpp as appUtilCpp
import src.codeGenerators.new_app.writers.appUtilHpp as appUtilHpp

from subprocess import call

def writeOperators(spbench_path, app_id, operators_list):
    """ !@brief Function for writing the app utils code

    This function is used to write the app utils code.

    @param spbench_path Path of the SPBench root.
    @param app_id Name of the application.
    @param operators_list List of operators.
    """

    # set directories
    templates_path = spbench_path + "/sys/apps/" +  app_id + "/templates"
    operators_path = templates_path + "/operators"
    source_path = operators_path + "/src"
    include_path = operators_path + "/include"

    # create a json file inside the operators folder with the operators list as keys
    operators_list_file = operators_path + "/operators.json"
    operators_dict = {}
    for operator in operators_list:
        operators_dict.update({operator:""})
    writeDicTo(operators_list_file, operators_dict)

    # ******************************************************************************************
    # create and write the code for app_id.cpp
    # ******************************************************************************************

    app_cpp = operators_path + "/" + app_id + ".cpp"

    # write a general header
    generalHeader.writeGeneralHeader(app_cpp, app_id + ".cpp", "This file contains the main function of the " + app_id + " application.")

    # write the code for app_id.cpp
    appCpp.writeAppCpp(app_cpp, app_id, operators_list)

    # ******************************************************************************************
    # create and write the code for the app_id.hpp
    # ******************************************************************************************

    app_hpp = operators_path + "/" + app_id + ".hpp"

    # write a general header
    generalHeader.writeGeneralHeader(app_hpp, app_id + ".hpp", "This file contains the header of the " + app_id + " application.")

    # write the code for app_id.hpp
    appHpp.writeAppHpp(app_hpp, app_id, operators_list)

    # ******************************************************************************************
    # create and write the code for the operators
    # ******************************************************************************************

    for operator in operators_list:
        operator_hpp = include_path + "/" + operator + "_op.hpp"
        operator_cpp = source_path + "/" + operator + "_op.cpp"

        # write a general header for operator_hpp
        generalHeader.writeGeneralHeader(operator_hpp, operator + "_op.hpp", "This file contains the header of the " + operator + " operator.")

        # write the code for operator_hpp
        operatorHpp.writeOperatorHpp(operator_hpp, operator, app_id)

        # write a general header for operator_cpp
        generalHeader.writeGeneralHeader(operator_cpp, operator + "_op.cpp", "This file contains the code of the " + operator + " operator.")

        # write the code for operator_cpp
        operatorCpp.writeOperatorCpp(operator_cpp, operator)

    # ******************************************************************************************



