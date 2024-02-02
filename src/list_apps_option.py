## 
 ##############################################################################
 #  File  : list_apps_option.py
 #
 #  Title : SPBench-CLI List Applications Option
 #
 #  Author: Adriano Marques Garcia <adriano1mg@gmail.com> 
 #
 #  Date  : July 06, 2021
 #
 #  Copyright (C) 2021 Adriano M. Garcia
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

import sys

from src.utils.utils import *
from src.utils.dict import *

#print all inputs
def list_apps_func(spbench_path):

    # get apps registry in dictionay format
    apps_registry = getAppsRegistry(spbench_path)

    # for each app in the registry, print name, type, number of operators, description, and notes

    print(" -------------------------------------------------------------------")
    for app in apps_registry:
        #print the main key (app_id)
        print(color.BOLD + " Application ID: " + color.END + str(app))
        #print the other keys
        for key in apps_registry[app]:
            print(" " + key + ": " + str(apps_registry[app][key]))
        print(" -------------------------------------------------------------------")

    print("")
    sys.exit()
