## 
 ##############################################################################
 #  File  : operatorHpp.py
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

# ******************************************************************************************
# create and write the header of a given operator
# ******************************************************************************************
def writeOperatorHpp(file_path, operator_id, app_id):
    """ !@brief Function for writing the header of a given operator

    This function is used to write the header of a given operator.

    @param file_path Path of the file.
    @param operator_id Name of the operator.
    @param app_id Name of the application.
    """

    tmpl = Template("""
#ifndef ${guard}_OP_HPP
#define ${guard}_OP_HPP

#include <${app}.hpp>

namespace spb{

/**
* @brief This method implements the encapsulation of the ${op} operator.
* 
* @param item (&item) 
*/
void ${op}::op(Item &item){

    #if !defined NO_LATENCY
        Metrics metrics;
        std::chrono::high_resolution_clock::time_point op_timestamp1;
        if(Metrics::latency_is_enabled()){
            op_timestamp1 = std::chrono::high_resolution_clock::now();
        }
    #endif

    unsigned int num_item = 0;

    while(num_item < item.batch_size){ //batch loop
        ${op}_op(item.item_batch[num_item]);
        num_item++;
    }

    #if !defined NO_LATENCY
        if(metrics.latency_is_enabled()){
            std::chrono::high_resolution_clock::time_point op_timestamp2 = std::chrono::high_resolution_clock::now();
            item.latency_op.push_back(std::chrono::duration_cast<std::chrono::duration<double>>(op_timestamp2 - op_timestamp1));
        }
    #endif
}

} // end of namespace spb

#endif /* ${guard}_OP_HPP */
""")

    output = tmpl.substitute(
        guard=operator_id.upper(),
        app=app_id,
        op=operator_id,
    )

    with open(file_path, 'a') as f:
        f.write(output)
        f.write("\n")

# ******************************************************************************************


