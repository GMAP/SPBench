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
from string import Template

    # ******************************************************************************************
    # create and write the header of the system-side headers of the application
    # ******************************************************************************************
def writeAppUtilsHpp(file_path, app_id, list_of_operators):
    """ !@brief Function for writing the header of the system-side headers of the application

    This function is used to write the header of the system-side headers of the application.

    @param file_path Path of the file.
    @param app_id Name of the application.
    @param list_of_operators List of operators.
    """
    
    prog_name = f"{app_id}_utils"
    num_operators = len(list_of_operators)

    template = Template("""
#ifndef ${prog_name_upper}_HPP
#define ${prog_name_upper}_HPP

/** Includes **/
#include <spbench.hpp>

namespace spb {

#define NUMBER_OF_OPERATORS ${num_operators}

struct item_data;
class Item;
class Source;
class Sink;

extern int some_external_variable;

void init_bench(int argc, char* argv[]);
void end_bench();

/* Data from external input */
struct workload_data {
    std::string some_input_file;
    int some_integer;
    std::string id;
};
extern workload_data input_data;

/* All the data communicated between operators */
struct item_data {
    std::string *some_pointer;
    std::string some_string;
    unsigned int some_unsigned_integer;
    std::vector<std::string> some_vector;
    unsigned int index;

    item_data():
        some_pointer(NULL),
        some_unsigned_integer(0)
    {};

    ~item_data(){
        some_vector.clear();
    }
};

/* This class implements an Item */
class Item : public Batch {
public:
    std::vector<item_data> item_batch;

    Item():Batch(NUMBER_OF_OPERATORS){};

    ~Item(){}
};

class Source {
public:
    static std::chrono::high_resolution_clock::time_point source_item_timestamp;
    static bool op(Item &item);
    Source(){}
    virtual ~Source(){}
};

class Sink {
public:
    static void op(Item &item);
    Sink(){}
    virtual ~Sink(){}
};

} // end of namespace spb

#endif
""")

    content = template.substitute(
        prog_name_upper=prog_name.upper(),
        num_operators=num_operators
    )

    with open(file_path, 'a') as app_hpp_utils_file:
        app_hpp_utils_file.write(content)
    # ******************************************************************************************
