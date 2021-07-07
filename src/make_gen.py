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

from src.bench_utils import *

from subprocess import call

def make_gen(stream_path, version_id, app_id, ppi_id):

    #if app_id == 'ferret':
    #    make_gen_ferret(version_id, app_id, ppi_id)
    #    sys.exit()
    
    programm_path = stream_path + "/apps/" +  app_id + "/" + ppi_id + "/" + version_id + "/"
    json_file =  programm_path + "config.json"
    
    #check if config.json file exists
    if os.path.exists(json_file) == False:
        print("   Error!\n-> Not found: [" + json_file + "]\n")
        print("-> Add your version using the 'new' option first.")
        sys.exit()

    #open JSON file
    json_info = open(json_file, 'r')
    with json_info as f:
        json_data = json.load(f)
    json_info.close()

    #set the full path of the version source file
    sourceFileName = version_id + ".cpp"
    source = programm_path + sourceFileName
    if os.path.exists(source) == False:
        print("   Error!\n-> Not found: [" + source + "]\n")
        print(" > Add your version using the 'new' option first.")
        #print "-> Check if you set the correct name of your .cpp in your config.json file."
        sys.exit()

    Makefile = open(programm_path+'Makefile', 'w')

    compiler = json_data["compiler"]
    compiler = compiler.replace('$BENCH_DIR', stream_path)
    Makefile.write('COMPILER='+compiler+'\n')

    pkg = json_data["pkgconfig"]
    if isNotBlank(pkg):
        pkg = pkg.replace('$BENCH_DIR', stream_path)
        Makefile.write('PKG=`' + pkg +'`\n')
    else:
        Makefile.write('PKG=\n')
    #pkg = pkg.replace('$BENCH_DIR', stream_path)
    #if bench_utils.isNotBlank(pkg):
    #    if os.path.exists(pkg) == False:
    #        print "   Error!\n-> Not found: [" + pkg + "]\n"
    #        print "-> Check if you set the correct location of your pkg configuration file in your config.json file."
    #        sys.exit()
    #    else:
    #        Makefile.write('PKG=`pkg-config --cflags --libs ' + pkg + '`\n')
    #else:
    #    Makefile.write('PKG=\n')

    flags = json_data["flags"]
    Makefile.write('FLAGS=' + flags + '\n')

    #print current_path
    #libs = json_data["libs_cmd"]
    includes = json_data["includes"]
    includes_cmd = ""
    for key in includes:
        includes[key] = includes[key].replace('$BENCH_DIR', stream_path)
        includes_cmd += includes[key] + " "
        
    include_bench_lib = '-I ' + stream_path + '/sys/apps/' + app_id + '/ '
    include_metrics = '-I ' + stream_path + '/libs/bench_metrics/ '
    include_upl = '-I ' + stream_path + '/libs/upl/include/upl/'

    Makefile.write('INCLUDES=' + includes_cmd + include_bench_lib + include_metrics + include_upl + '\n')

    libs = json_data["libs"]
    libs_cmd = ""
    for key in libs:
        libs[key] = libs[key].replace('$BENCH_DIR', stream_path)
        libs_cmd += libs[key] + " "

    Makefile.write('LIBS=' + libs_cmd + '-L ' + stream_path + '/libs/upl/lib/ -lupl\n')

    macros = json_data["macros"]
    Makefile.write('MACRO='+macros+'\n\n')

    Makefile.write('all: ' + version_id + '\n\n')

    Makefile.write(version_id + ': ' + sourceFileName + '\n')

    bin_path = stream_path + "/bin/" + app_id + "/" + ppi_id + "/" + version_id

    pre_src_cmd = json_data["pre_src_cmd"]
    pre_src_cmd = pre_src_cmd.replace('$BENCH_DIR', stream_path)
    
    post_src_cmd = json_data["post_src_cmd"]
    post_src_cmd = post_src_cmd.replace('$BENCH_DIR', stream_path)
    
    Makefile.write('\t$(COMPILER) $(FLAGS) $(INCLUDES) ' + pre_src_cmd + ' $^ -o ' + bin_path + ' ' + post_src_cmd + ' $(PKG) $(LIBS) $(MACRO)\n\n')

    Makefile.write('clean:\n\trm -f ' + bin_path + '\n')
    Makefile.close()
    #print("-> '" + version_id + "' successfully registered!!\n")

def make_gen_ferret(stream_path, version_id, app_id, ppi_id):

    programm_path = stream_path + "/apps/" +  app_id + "/" + ppi_id + "/" + version_id + "/"
    json_file =  programm_path + "config.json"

    #check if config.json file exists
    if os.path.exists(json_file) == False:
        print("   Error!\n-> Not found: [" + json_file + "]\n")
        print("-> Add your version using the 'new' option first.")
        sys.exit()

    #open JSON file
    json_info = open(json_file, 'r')
    with json_info as f:
        json_data = json.load(f)
    json_info.close()

    #set the full path of the version source file
    sourceFileName = version_id + ".cpp"
    source = programm_path + sourceFileName
    if os.path.exists(source) == False:
        print("   Error!\n-> Not found: [" + source + "]\n")
        print("-> Add your version using the 'new' option first.")
        #print "-> Check if you set the correct name of your .cpp in your config.json file."
        sys.exit()

    compiler = json_data["compiler"]
    compiler = compiler.replace('$BENCH_DIR', stream_path)

    pre_src_cmd = json_data["pre_src_cmd"]
    pre_src_cmd = pre_src_cmd.replace('$BENCH_DIR', stream_path)

    post_src_cmd = json_data["post_src_cmd"]
    post_src_cmd = post_src_cmd.replace('$BENCH_DIR', stream_path)

    cflags = json_data["cflags"]
    ldflags = json_data["ldflags"]

    macros = json_data["macros"]

    pkgconfig = json_data["pkg-config"]
    pkg_cmd = ""
    for key in pkgconfig:
        if isNotBlank(pkgconfig[key]):
            pkgconfig[key] = pkgconfig[key].replace('$BENCH_DIR', stream_path)
            pkg_cmd += '`'+pkgconfig[key]+'` '

    includes = json_data["includes"]
    includes_cmd = ""
    for key in includes:
        if isNotBlank(includes[key]):
            includes[key] = includes[key].replace('$BENCH_DIR', stream_path)
            includes_cmd += includes[key] + " "
        
    include_bench_lib = '-I ' + stream_path + '/sys/apps/' + app_id + '/ '
    include_metrics = '-I ' + stream_path + '/libs/bench_metrics/ '
    include_upl = '-I ' + stream_path + '/libs/upl/include/upl/'

    libs = json_data["libs"]
    libs_cmd = ""
    for key in libs:
        if isNotBlank(libs[key]):
            libs[key] = libs[key].replace('$BENCH_DIR', stream_path)
            libs_cmd += libs[key] + " "

    Makefile = open(programm_path+'Makefile', 'w')

    Makefile.write('BENCH=' + stream_path + '\n')
    Makefile.write('APP_ROOT=$(BENCH)/sys/apps/ferret\n\n')

    Makefile.write('.PHONY:    all clean dirs ferret benchmark\n\n')

    Makefile.write('SRCDIR=$(APP_ROOT)/src\n')
    Makefile.write('INCDIR=$(APP_ROOT)/include\n')
    Makefile.write('BINDIR=$(BENCH)/bin/ferret/' + ppi_id + '\n')
    Makefile.write('LIBDIR=$(APP_ROOT)/parsec/lib\n')
    Makefile.write('OBJDIR=$(APP_ROOT)/parsec/obj\n\n')

    Makefile.write('COMPILER=' + compiler + '\n')

    Makefile.write('CFLAGS= ' + cflags + '\n')

    Makefile.write('INCLUDES= -I$(INCDIR) -I $(APP_ROOT) ' + includes_cmd + include_bench_lib + include_metrics + include_upl + '\n')
    Makefile.write('LIBS= -L$(LIBDIR) ' + pkg_cmd + ' ' + libs_cmd + '-L ' + stream_path + '/libs/upl/lib/ -lupl\n')

    Makefile.write('LDFLAGS= -lrt -lm -lcass -lstdc++ ' + ldflags + '\n')

    Makefile.write('MACRO='+macros+'\n\n')

    Makefile.write('PRE_SRC_CMD=' + pre_src_cmd + '\n')
    Makefile.write('POS_SRC_CMD=' + post_src_cmd + '\n\n')

    Makefile.write('VPATH = $(APP_ROOT)/src:$(APP_ROOT)/src/lsh:$(APP_ROOT)/tools:$(APP_ROOT)/image\n\n')

    Makefile.write('export LIBDIR BINDIR OBJDIR CC CXX CFLAGS LDFLAGS LIBS INCLUDES\n\n')

    Makefile.write('all: dirs ferret benchmark\n\n')
    Makefile.write('dirs:\n\tmkdir -p $(BINDIR)\n\n')
    Makefile.write('ferret:\n\t(cd $(APP_ROOT); make -j$(nproc))\n\n')

    Makefile.write('benchmark_base := ' + version_id + '\n')
    Makefile.write('benchmark_src := $(benchmark_base).cpp\n')
    Makefile.write('benchmark_tgt := $(notdir $(benchmark_src))\n')
    Makefile.write('benchmark_tgt := $(addprefix $(BINDIR)/, $(benchmark_tgt:.cpp=))\n\n')

    Makefile.write('benchmark : $(benchmark_tgt)\n\n')

    Makefile.write('clean:\n\t-rm $(OBJDIR)/*\n\n')

    Makefile.write('$(OBJDIR)/' + version_id + '.o: ' + version_id + '.cpp\n')
    Makefile.write('\t$(COMPILER) $(CFLAGS) $(INCLUDES) -c $(PRE_SRC_CMD) $^ -o $@ $(POS_SRC_CMD) $(LIBS) $(MACRO) $(LDFLAGS)\n\n')

    Makefile.write('$(benchmark_tgt): $(BINDIR)/% : $(OBJDIR)/%.o $(LIBDIR)/libcass.a $(LIBDIR)/libcassimage.a\n')
    Makefile.write('\t@echo "   CXX \'$<\' ==> \'$@\'"\n')
    Makefile.write('\t@$(CXX) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LIBS) $(LDFLAGS)\n')

    Makefile.close()
    #sys.exit()