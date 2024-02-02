## 
 ##############################################################################
 #  File  : write_operators.py
 #
 #  Title : SPBench operators writer
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
import src.codeGenerators.new_app.writers.generalHeader as generalHeader
import src.codeGenerators.new_app.writers.appCpp as appCpp
import src.codeGenerators.new_app.writers.appHpp as appHpp

def writeApp(spbench_path, app_id, operators_list):
    """ !@brief Function for writing the application code (user-side)

    This function is used to write the application code (user-side).

    @param spbench_path Path of the SPBench root.
    @param app_id Name of the application.
    @param operators_list List of operators.
    """

    # set directories
    templates_path = spbench_path + "/sys/apps/" +  app_id + "/templates"

    # ******************************************************************************************
    # create and write the code for app_id.cpp
    # ******************************************************************************************

    app_cpp = templates_path + "/" + app_id + ".cpp"

    # write a general header
    generalHeader.writeGeneralHeader(app_cpp, app_id + ".cpp", "This file contains the main function of the " + app_id + " application.")

    # write the code for app_id.cpp
    appCpp.writeAppCpp(app_cpp, app_id, operators_list)

    # ******************************************************************************************
    # create and write the code for the app_id.hpp
    # ******************************************************************************************

    app_hpp = templates_path + "/" + app_id + ".hpp"

    # write a general header
    generalHeader.writeGeneralHeader(app_hpp, app_id + ".hpp", "This file contains the header of the " + app_id + " application.")

    # write the code for app_id.hpp
    appHpp.writeAppHpp(app_hpp, app_id, operators_list)

    # *******************************************************************************
    # create the json configuration file
    # *******************************************************************************
    config_file = {
        "CXX": "g++ -std=c++1y",
        "CXX_FLAGS":"",
        "PPI_CXX": "g++ -std=c++1y",
        "PPI_CXX_FLAGS":"",
        "PRE_SRC_CMD": "",
        "POST_SRC_CMD": "",
        "MACROS": "",
        "PKG-CONFIG": {
                "myPKG_1": "",
                "myPKG_2": "",
                "myPKG_N": ""
            },
        "INCLUDES": {
                "myINC_1": "",
                "myINC_2": "",
                "myINC_N": "",
                "UPL" : "-I $SPB_HOME/libs/upl/include/upl/"
        },
        "LIBS": {
                "myLIB_1": "",
                "myLIB_2": "",
                "myLIB_N": "",
                "UPL" : "-L $SPB_HOME/libs/upl/lib/x86 -lupl"
        },
        "LDFLAGS": ""
    }

    writeDicTo(templates_path + "/config.json", config_file)

    # *******************************************************************************

