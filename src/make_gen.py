## 
 ##############################################################################
 #  File  : make_gen.py
 #
 #  Title : SPBench makefile generator
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
import os
import json

from src.utils import *

from subprocess import call

def make_gen(stream_path, selected_benchmark):
    """
    This function reads a JSON configuration file from a given benchmark
    and writes a custom Makefile based on the parameters
    """
    app_id = selected_benchmark["app_id"]
    ppi_id = selected_benchmark["ppi_id"]
    bench_id = selected_benchmark["bench_id"]
    nsources = selected_benchmark["nsources"]

    ns = ""
    if(nsources):
        ns = "_ns"
    
    programm_path = stream_path + "/apps/" +  app_id + "/" + ppi_id + "/" + bench_id + "/"
    #bin_path = stream_path + "/bin/" + app_id + "/" + ppi_id + "/" + bench_id

    json_file =  programm_path + "config.json"
    
    #check if config.json file exists
    if os.path.exists(json_file) == False:
        print("   Error!\n-> Not found: [" + json_file + "]\n")
        print("-> Add your benchmark using the 'new' option first.")
        sys.exit()

    #open JSON file
    json_info = open(json_file, 'r')
    with json_info as f:
        json_data = json.load(f)
    json_info.close()

    #set the full path of the benchmark source file
    sourceFileName = bench_id + ".cpp"
    source = programm_path + sourceFileName
    if os.path.exists(source) == False:
        print("   Error!\n-> Not found: [" + source + "]\n")
        print(" > Add your benchmark using the 'new' option first.")
        sys.exit()
     
    ##########################################
    # Retrieve data from the JSON file
    ##########################################
    compiler = json_data["CXX"]
    compiler = compiler.replace('$BENCH_DIR', stream_path)

    compiler_flags = json_data["CXX_FLAGS"]
    compiler_flags = compiler_flags.replace('$BENCH_DIR', stream_path)

    # if no specific compiler were defined, use the general one
    if not json_data["PPI_CXX"]:
        ppi_compiler = compiler
    else:
        ppi_compiler = json_data["PPI_CXX"]
        ppi_compiler = ppi_compiler.replace('$BENCH_DIR', stream_path)

    ppi_compiler_flags = json_data["PPI_CXX_FLAGS"]
    ppi_compiler_flags = ppi_compiler_flags.replace('$BENCH_DIR', stream_path)

    pre_src_cmd = json_data["PRE_SRC_CMD"]
    pre_src_cmd = pre_src_cmd.replace('$BENCH_DIR', stream_path)

    post_src_cmd = json_data["POST_SRC_CMD"]
    post_src_cmd = post_src_cmd.replace('$BENCH_DIR', stream_path)

    macros = json_data["MACROS"]

    pkgconfig = json_data["PKG-CONFIG"]
    pkg_cmd = []
    for key in pkgconfig:
        if isNotBlank(pkgconfig[key]):
            pkgconfig[key] = pkgconfig[key].replace('$BENCH_DIR', stream_path)
            pkg_cmd.append('`'+pkgconfig[key]+'` ')

    includes = json_data["INCLUDES"]
    includes_cmd = []
    for key in includes:
        if isNotBlank(includes[key]):
            includes[key] = includes[key].replace('$BENCH_DIR', stream_path)
            includes_cmd.append(includes[key] + " ")
        
    # Some default includes required by SPBench
    includes_cmd.append('-I $(BENCH_DIR) ')
    includes_cmd.append('-I $(SRC_DIR) ')
    includes_cmd.append('-I $(SRC_DIR)/include ')
    includes_cmd.append('-I $(SPB_LIB_DIR) ')
    #includes_cmd.append('-I $(SPB_HOME)/libs/upl/include/upl/ ')
    includes_cmd.append('-I $(BENCH_DIR)/operators/include/ ')
    
    libs = json_data["LIBS"]
    libs_cmd = []
    for key in libs:
        if isNotBlank(libs[key]):
            libs[key] = libs[key].replace('$BENCH_DIR', stream_path)
            libs_cmd.append(libs[key] + " ")

    #libs_cmd.append('-L $(SPB_HOME)/libs/upl/lib/ -lupl ')

    if app_id == 'ferret':
        libs_cmd.append('-L $(SRC_DIR)/parsec/lib ')

    ldflags = json_data["LDFLAGS"]
    if app_id == 'ferret':
        ldflags += ' -lrt -lm -lcass -lstdc++ -ljpeg -lgsl -lgslcblas'

    ###########################################
    # Makefile writing
    ###########################################
    Makefile = open(programm_path+'Makefile', 'w')

    Makefile.write('SPB_HOME := ' + stream_path + '\n\n')
    
    Makefile.write('CXX := ' + compiler + '\n')
    Makefile.write('CXX_FLAGS := ' + compiler_flags + '\n\n')
    
    Makefile.write('PPI_CXX := ' + ppi_compiler + '\n')
    Makefile.write('PPI_CXX_FLAGS := ' + ppi_compiler_flags + '\n\n')

    Makefile.write('BENCHMARK := ' + bench_id + '\n')
    Makefile.write('APP_ID := ' + app_id + '\n')
    Makefile.write('PPI_ID := ' + ppi_id + '\n\n')

    if app_id == 'ferret':
        Makefile.write('.PHONY:\t clean all ferret $(BENCHMARK)\n\n')
    else:    
        Makefile.write('.PHONY:\t clean all $(BENCHMARK)\n\n')

    Makefile.write('SRC_DIR := $(SPB_HOME)/sys/apps/$(APP_ID)\n')
    Makefile.write('BENCH_DIR := $(SPB_HOME)/apps/$(APP_ID)/$(PPI_ID)/$(BENCHMARK)\n')
    Makefile.write('SPB_LIB_DIR := $(SPB_HOME)/libs/spbench\n\n')

    Makefile.write('BIN_DIR := $(SPB_HOME)/bin/$(APP_ID)/$(PPI_ID)\n')

    Makefile.write('SRC_PATH := ./operators/src\n')
    Makefile.write('OBJ_PATH := ./operators/obj\n')
    Makefile.write('SRC := $(wildcard $(SRC_PATH)/*.cpp)\n')
    Makefile.write('OBJ_FILES := $(SRC:.cpp=.o)\n')
    Makefile.write('OBJ := $(subst $(SRC_PATH),$(OBJ_PATH),$(OBJ_FILES))\n\n')

    Makefile.write('PRE_SRC_CMD := ' + pre_src_cmd + '\n')
    Makefile.write('POS_SRC_CMD := ' + post_src_cmd + '\n')
    Makefile.write('LDFLAGS := ' + ldflags + '\n')
    Makefile.write('MACROS := ' + macros + '\n\n')

    if includes_cmd:
        Makefile.write('INCLUDES := ')
        for include in includes_cmd:
            Makefile.write(include + '\\')
            Makefile.write('\n\t')

    if pkg_cmd:
        Makefile.write('\nPKG := ')
        for pkg in pkg_cmd:
            Makefile.write(pkg + '\\')
            Makefile.write('\n\t')

    if libs_cmd:
        Makefile.write('\nLIBS := ')
        for lib in libs_cmd:
            Makefile.write(lib + '\\')
            Makefile.write('\n\t')

    if app_id == 'ferret':
        Makefile.write('\nVPATH = $(SRC_DIR)/src:$(SRC_DIR)/src/lsh:$(SRC_DIR)/tools:$(SRC_DIR)/image\n\n')
        
        Makefile.write('LIBDIR=$(SRC_DIR)/parsec/lib\n')
        Makefile.write('OBJDIR=$(SRC_DIR)/parsec/obj\n\n')

        Makefile.write('export LIBDIR OBJDIR\n\n')
        
        Makefile.write('all:	ferret $(BENCHMARK)\n\n')
        
        Makefile.write('ferret:\n')
        Makefile.write('\t(cd $(SRC_DIR); make -j$(nproc))')

    Makefile.write('\n\n$(BENCHMARK): $(APP_ID).a\n')
    Makefile.write('\t@echo "   $(CXX) $< ==> $@"\n')
    Makefile.write('\t@mkdir -p $(BIN_DIR)\n')

    if app_id == 'ferret':
        Makefile.write('\t@$(CXX) $(CFLAGS) $(INCLUDES) $^ -o $(BIN_DIR)/$@ $(LIBDIR)/libcass.a $(LIBDIR)/libcassimage.a $(PKG) $(LIBS) $(LDFLAGS) $(MACROS)\n\n')
    else:    
        Makefile.write('\t@$(CXX) $(CFLAGS) $(INCLUDES) $^ -o $(BIN_DIR)/$@ $(PKG) $(LIBS) $(LDFLAGS) $(MACROS)\n\n')

    Makefile.write('$(APP_ID).a: $(BENCHMARK).o $(SPB_LIB_DIR)/spbench.o $(SRC_DIR)/$(APP_ID)_utils' + ns + '.o $(OBJ)\n')
    Makefile.write('\t@echo "   ar  $^ ==> $@"\n')
    Makefile.write('\t@ar -crs $@ $^\n\n')

    Makefile.write('$(SPB_LIB_DIR)/spbench.o: $(SPB_LIB_DIR)/spbench.cpp\n')
    Makefile.write('\t@echo "   $(CXX) $< ==> $@"\n')
    Makefile.write('\t@$(CXX) $(CFLAGS) $(INCLUDES) -c $^ -o $@ $(PKG) $(LIBS) $(LDFLAGS) $(MACROS)\n\n')

    Makefile.write('$(SRC_DIR)/$(APP_ID)_utils' + ns + '.o: $(SRC_DIR)/$(APP_ID)_utils' + ns + '.cpp\n')
    Makefile.write('\t@echo "   $(CXX) $< ==> $@"\n')
    Makefile.write('\t@$(CXX) $(CFLAGS) $(INCLUDES) -c $^ -o $@ $(PKG) $(LIBS) $(LDFLAGS) $(MACROS)\n\n')

    Makefile.write('$(BENCHMARK).o: $(BENCHMARK).cpp\n')
    Makefile.write('\t@echo "   $(PPI_CXX) $< ==> $@"\n')
    Makefile.write('\t@$(PPI_CXX) $(PPI_CXX_FLAGS) $(INCLUDES) -c $(PRE_SRC_CMD) $^ $(POS_SRC_CMD) $(PKG) $(LIBS) $(LDFLAGS) $(MACROS)\n\n')

    Makefile.write('$(OBJ_PATH)/%.o : $(SRC_PATH)/%.cpp\n')
    Makefile.write('\t@echo "   $(PPI_CXX) $< ==> $@"\n')
    Makefile.write('\t@mkdir -p $(OBJ_PATH)\n')
    Makefile.write('\t@$(PPI_CXX) $(PPI_CXX_FLAGS) $(INCLUDES) -c $(PRE_SRC_CMD) $^ $(POS_SRC_CMD) -o $@ $(PKG) $(LIBS) $(LDFLAGS) $(MACROS)\n\n')

    Makefile.write('.PHONY: clean\n')
    Makefile.write('clean:\n')
    Makefile.write('\trm -f $(SRC_DIR)/parsec/obj/*\n')
    Makefile.write('\trm -f $(OBJ) $(BENCHMARK).o $(SRC_DIR)/$(APP_ID)_utils' + ns + '.o $(APP_ID).a $(SPB_LIB_DIR)/spbench.o $(BIN_DIR)/$(BENCHMARK)\n')
    Makefile.write('\trm -rf $(OBJ_PATH)\n\n')

    Makefile.close()
