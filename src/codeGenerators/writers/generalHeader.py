## 
 ##############################################################################
 #  File  : generalHeader.py
 #
 #  Title : SPBench general header writer
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

def writeGeneralHeader(file, file_name, description):
    """ !@brief Function for writing a general header for the files

    This function is used to write a general header for the files.

    @param file File to be written.
    @param file_name Name of the file.
    @param description Description of the file.
    """

    # ******************************************************************************************
    # create and write a general header for the files
    # ******************************************************************************************

    # open the app_cpp file
    app_cpp_file = open(file, 'w')

    # write the app_cpp file
    app_cpp_file.write("/*\n")
    app_cpp_file.write(" *  File  : " + file_name + "\n")
    app_cpp_file.write(" *\n")
    app_cpp_file.write(" *  Description: " + description + "\n")
    app_cpp_file.write(" *\n")
    app_cpp_file.write(" *  Author: Adriano Marques Garcia\n")
    app_cpp_file.write(" *  Created on: " + date.today().strftime("%B %d, %Y") + "\n")
    app_cpp_file.write(" *\n")
    app_cpp_file.write(" *  License: GNU General Public License v3.0\n")
    app_cpp_file.write(" *\n")
    app_cpp_file.write(" *  This program is free software: you can redistribute it and/or modify\n")
    app_cpp_file.write(" *  it under the terms of the GNU General Public License as published by\n")
    app_cpp_file.write(" *  the Free Software Foundation, either version 3 of the License, or\n")
    app_cpp_file.write(" *  (at your option) any later version.\n")
    app_cpp_file.write(" *\n")
    app_cpp_file.write(" *  This program is distributed in the hope that it will be useful,\n")
    app_cpp_file.write(" *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n")
    app_cpp_file.write(" *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n")
    app_cpp_file.write(" *  GNU General Public License for more details.\n")
    app_cpp_file.write(" *\n")
    app_cpp_file.write(" *  You should have received a copy of the GNU General Public License\n")
    app_cpp_file.write(" *  along with this program. If not, see <https://www.gnu.org/licenses/>.\n")
    app_cpp_file.write(" */\n")

    app_cpp_file.close()
    # ******************************************************************************************



