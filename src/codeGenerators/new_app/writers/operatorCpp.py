## 
 ##############################################################################
 #  File  : operatorCpp.py
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


def writeOperatorCpp(file_path, operator_id):
    """ !@brief Function for writing the source code of an example operator

    This function is used to write the source code of an example operator.

    @param file_path Path of the file.
    @param operator_id Name of the operator.
    """

    # ******************************************************************************************
    # create and write an example code for a operator
    # ******************************************************************************************
    template = Template("""
#include <../include/${operator_id}_op.hpp>

/**
 * @brief This function implements the ${operator_id} operator.
 * 
 * @param item (&item) 
 */
inline void spb::${operator_id}::${operator_id}_op(spb::item_data &item){

    // All the code inside this function is only an usage example.
    // Here we are just using the data item to do some basic and nonsense operations.
    // You can delete it and write your own operator as you wish.

    double some_double = 0.0;
    bool some_bool = true;

    item.some_vector.push_back(std::to_string(some_double));

    for (std::vector<std::string>::const_iterator iterator = item.some_vector.begin();
         iterator != item.some_vector.end(); iterator++, item.some_unsigned_integer++) {
        some_double = spb::Metrics::items_at_source_counter / some_external_variable;
        if (some_bool) {
            item.some_unsigned_integer += some_external_variable;
        }
    }

    if (item.some_unsigned_integer % 2 == 0) {
        std::cout << spb::Metrics::items_at_source_counter 
                  << " - This is a print message. It will not be printed in real time during the execution, unless you use the -debug flag."
                  << std::endl;
    }

    usleep(100000);
    item.some_vector.push_back(std::to_string(some_double));
}
""")

    content = template.substitute(operator_id=operator_id)

    with open(file_path, 'a') as op_cpp_file:
        op_cpp_file.write(content)


    # ******************************************************************************************



