## 
 ##############################################################################
 #  File  : appCpp.py
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
from string import Template

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

    main_template = Template("""
#include <${app_id}.hpp>

int main (int argc, char* argv[]) {
    spb::init_bench(argc, argv); // Initializations
    spb::Metrics::init();
    while(1) {
        spb::Item item;
        if (!spb::Source::op(item)) break;
$operator_calls
        spb::Sink::op(item);
    }
    spb::Metrics::stop();
    spb::end_bench();
    return 0;
}
""")

    # Generate the operator call lines
    operator_calls = "\n".join([f"\t\tspb::{op}::op(item);" for op in operators_list])

    content = main_template.substitute(app_id=app_id, operator_calls=operator_calls)

    with open(file_path, 'a') as app_cpp_file:
        app_cpp_file.write(content)
    # ******************************************************************************************



