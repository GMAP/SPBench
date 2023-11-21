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

from src.utils.shell import *
from src.utils.dict import *
import src.codeGenerators.new_app.writers.appUtilCpp as appUtilCpp
import src.codeGenerators.new_app.writers.appUtilHpp as appUtilHpp
import src.codeGenerators.new_app.writers.generalHeader as generalHeader

def writeAppUtils(spbench_path, app_id, operators_list):
    """ !@brief Function for writing the app utils code

    This function is used to write the app utils code.

    @param spbench_path Path of the SPBench root.
    @param app_id Name of the application.
    @param operators_list List of operators.
    """

    # set directories
    app_path = spbench_path + "/sys/apps/" +  app_id

    prog_name = app_id + "_utils"


    # ******************************************************************************************
    # create and write the code for app_id_utils.cpp
    # ******************************************************************************************

    app_cpp = app_path + "/" + prog_name + ".cpp"

    # write a general header
    generalHeader.writeGeneralHeader(app_cpp, prog_name + ".cpp", "This file contains the hidden functions of the " + app_id + " application.")

    # write the code for app_id_utils.cpp
    appUtilCpp.writeAppUtilsCpp(spbench_path, app_cpp, app_id, operators_list)

    # ******************************************************************************************
    # create and write the code for the app_id_utils.hpp
    # ******************************************************************************************

    app_hpp = app_path + "/" + prog_name + ".hpp"

    # write a general header
    generalHeader.writeGeneralHeader(app_hpp, prog_name + ".hpp", "This file contains the header for the hidden elements of the " + app_id + " application.")

    # write the code for app_id_utils.hpp
    appUtilHpp.writeAppUtilsHpp(app_hpp, prog_name, operators_list)



