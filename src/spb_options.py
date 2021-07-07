## 
 ##############################################################################
 #  File  : spb_options.py
 #
 #  Title : SPBench commands manager
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
from src.make_gen import *

from sys import version_info 
python_3 = version_info[0]

class color:
    PURPLE = '\033[95m'
    CYAN = '\033[96m'
    DARKCYAN = '\033[36m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    END = '\033[0m'

# list of SPBench reserved words
reserved_words = {'all'}

def install_func(stream_path, args):

    print(" ---------------------------------------------------------------")
    print(color.BOLD + "                         >> ATTENTION <<                         " + color.END)
    print(" ---------------------------------------------------------------\n")
    print(" Before proceeding with the installation you\n need to ensure that you have the following\n packages installed on your system:")
    print("    - build-essential")
    print("    - cmake")
    print("    - yasm")
    print("    - python\n")
    print(" To install them, you can run:\n")
    print(" sudo apt-get install -y build-essential cmake yasm python3.\n")
    print(" Or ask a system admin if you don't have sudo access.")
    print("---------------------------------------------\n")

    print(" You are installing dependencies for the following\n application:")
    print("\n -> " + args.app_id + "\n")
    if(args.app_id == "lane_detection"):# or (args.app_id == "person_recognition"):
        print(" This will install FastFlow, SPar, TBB, FFMPEG and OpenCV.\n")
        print(" >> It may take several minutes. <<")
    elif(args.app_id == "ferret"):
        print(" This will install FastFlow, SPar, TBB, GSL and JPEG.\n")
        print(" It may take a few minutes.")
    elif(args.app_id == "bzip2"):
        print(" This will install FastFlow, SPar, TBB, BZLIB.\n")
        print(" It may take a few moments.")
    else:
        print(" This will install FastFlow, SPar, TBB,\n BZLIB, FFMPEG, OpenCV, GSL, and JPEG.\n")
        print(" >> It may take several minutes. <<")
        
    # user input support for python 2 and 3
    if(python_3 == 3):
        answer = input("\nDo you want to proceed with the installation? [yes/no]\n")
    else:
        answer = raw_input("\nDo you want to proceed with the installation? [yes/no]\n")

    if(answer.lower() not in ["y","yes"]):
        print("Installation canceled!\n")
        sys.exit()

    os.system('bash ' + stream_path + '/ppis/install_ppis.sh')
    os.system('bash ' + stream_path + '/libs/install_libs.sh ' + args.app_id)

    print(" ---------------------------------------------------------------")
    print(color.BOLD + "                         >> IMPORTANT <<                         " + color.END)
    print(" ---------------------------------------------------------------\n")
    print(" Now you must run \'source setup_vars.sh\'\n to load the libraries.\n")
    print(" Obs: You must load the libraries before using\n SPBench every time you star a new session!")
    print(" ---------------------------------------------------------------\n")

    sys.exit()

#add a new version to the suite
def new_func(stream_path, args):

    # check if it is not a reserved word
    if args.version_id in reserved_words:
        print("\n " + args.version_id + " is a SPBench reserved word")
        print(" You can not use it to name a version.\n")
        sys.exit()

    # set the path for the new version
    new_path = stream_path + "/apps/" +  args.app_id + "/" + args.ppi_id + "/" + args.version_id + "/"

    # if path already exists, exit
    #if os.path.isdir(new_path) == True: 
    #    print("\n Version \'" + args.version_id + "\' files already exists.\n")
    #    print(" To delete it, you must manually delete them\n")
    #    sys.exit()
    
    # set the path of the copy-from version
    ok_to_copy = False
    copy_path = ''
    copy_from = ''
    if not args.copy_from: # if no copy-from version was selected, set the copy-from path to the sequencial template
        copy_path = (stream_path + '/sys/apps/' + args.app_id + '/template/')
        copy_from = args.app_id + "_sequential"
        ok_to_copy = True
    else: # search for the selected version to make a new copy
        registry = open(stream_path + '/sys/registry.data', 'r')
        for line in registry:
            if isNotBlank(line):
                aux = line.split()
                version_id = aux[0]
                app_id = aux[1]
                ppi_id = aux[2]
                if version_id == args.copy_from:
                    if app_id != args.app_id: 
                        print("\n "+color.BOLD+"WARNING"+color.END+": You are trying to make a new copy from a different application.\n")
                        print("  Selected application: " + args.app_id)
                        print(" Copy-from application: " + app_id)
                        # user input support for python 2 and 3
                        if(python_3 == 3):
                            answer = input("\n Do you want to proceed? [yes/no]\n")
                        else:
                            answer = raw_input("\n Do you want to proceed? [yes/no]\n")

                        if(answer.lower() not in ["y","yes"]): # delete the old version
                            print(" Operation canceled!\n")
                            sys.exit()
                    copy_path = stream_path + "/apps/" +  app_id + '/' + ppi_id + '/' + args.copy_from + "/"
                    if os.path.exists(copy_path + args.copy_from + '.cpp'):
                        if os.path.exists(copy_path + 'config.json'):
                            copy_from = args.copy_from
                            ok_to_copy = True
                        else:
                            print(copy_path + 'config.json is missing!\n')
                    else:
                        print(copy_path + args.copy_from + '.cpp file is missing!\n')
        registry.close()

    # check if this version name was not previously given to other application
    registry_file = open(stream_path + '/sys/registry.data', 'r')
    for line in registry_file:
        if isNotBlank(line):
            aux = line.split()
            version_id = aux[0]
            app_id = aux[1]
            ppi_id = aux[2]
            if (version_id == args.version_id): # Tell the user if version id already exists
                print(" ---------------------------------------------------------------")
                print(color.BOLD + "                            >> WARNING <<                         " + color.END)
                print(" ---------------------------------------------------------------")
                print("\n This version name already exists!\n")
                print(" Version: " + version_id)
                print("     App: " + app_id)
                print("     PPI: " + ppi_id)
                if app_id != args.app_id: 
                    print("\n "+color.BOLD+"CAUTION"+color.END+": You are trying to register this version name under a\n different application: " + args.app_id)
                if ppi_id != args.ppi_id:
                    print("\n "+color.BOLD+"CAUTION"+color.END+": You are trying to register this version name under a\n different PPI: " + args.ppi_id)
                
                print("\n The existent version will be "+color.BOLD+"deleted"+color.END+" and replaced by:\n")
                print(" Version: " + args.version_id)
                print("     App: " + args.app_id)
                print("     PPI: " + args.ppi_id)

                # user input support for python 2 and 3
                if(python_3 == 3):
                    answer = input("\n Do you want to proceed? [yes/no]\n")
                else:
                    answer = raw_input("\n Do you want to proceed? [yes/no]\n")

                if(answer.lower() in ["y","yes"]): # delete the old version
                    delete_reg_func(stream_path, args)
                else:
                    print(" Operation canceled!\n")
                    registry_file.close()
                    sys.exit()
    registry_file.close()

    # if it is all ok to create a new version, do it
    if ok_to_copy:
        os.makedirs(new_path) # copy the files from somewhere else (template or existent version)
        os.system('cp ' + copy_path + copy_from +".cpp " + new_path + args.version_id + ".cpp")
        os.system('cp ' + copy_path + "config.json " + new_path + "config.json")
    else:
        print('\n Error while creating your new version\n')
        sys.exit()

    # prepare a new register to write into the registry file
    new_item = args.version_id + ' ' + args.app_id + ' ' + args.ppi_id + '\n'

    # write the new register into a existent registry 
    if os.path.exists(stream_path + '/sys/registry.data'):
        new_file = open(stream_path + '/sys/registry.tmp', 'w')
        new_file.write(new_item)

        old_file = open(stream_path + '/sys/registry.data', 'r')
        for line in old_file:
            if isNotBlank(line):
                aux = line.split()
                if aux[0] != args.version_id: # copy all registers from the old registry to the new one, but not the old register
                    new_file.write(line)

        os.remove(stream_path + '/sys/registry.data')
        os.rename(stream_path + '/sys/registry.tmp', 'sys/registry.data')
        new_file.close()
        old_file.close()
    else: # if there is no registry yet, create one and write the register
        new_file = open(stream_path + '/sys/registry.data', 'w')
        new_file.write(new_item)
        new_file.close()
    
    print("\n New version called \'" + args.version_id + "\' successfully added!\n")
    print(" ---------------------------------------------------------------")
    print(color.BOLD + "                         >> ATTENTION <<                         " + color.END)
    print(" ---------------------------------------------------------------")
    print(" To add your modifications into the new version, run:\n")
    print(" "+color.BOLD+"./bench.py edit -version " + args.version_id + " -editor [text_editor]\n\n"+color.END)
    print(" You must also configure it to add any required dependencies:\n")
    print(" "+color.BOLD+"./bench.py configure -version " + args.version_id + " -editor [text_editor]"+color.END)
    print(" ---------------------------------------------------------------")

    sys.exit()

def register_func(stream_path):

    registry = open(stream_path + '/sys/registry.data', 'r')
    for line in registry:
        if isNotBlank(line):
            aux = line.split()
            version_id = aux[0]
            app_id = aux[1]
            ppi_id = aux[2]
            if(app_id == 'ferret'):
                make_gen_ferret(stream_path, version_id, app_id, ppi_id)
            else:
                make_gen(stream_path, version_id, app_id, ppi_id)
    registry.close()

def edit_source_func(stream_path, args):
    registry = open(stream_path + '/sys/registry.data', 'r')
    for line in registry:
        if isNotBlank(line):
            aux = line.split()
            version_id = aux[0]
            app_id = aux[1]
            ppi_id = aux[2]
            if version_id == args.version_id:
                cmd_line = args.user_editor + " " + stream_path + "/apps/" + app_id + "/" + ppi_id + "/" + args.version_id + "/" + args.version_id + ".cpp"
                print("Running >> " + cmd_line)
                os.system(cmd_line)
                if(aux[1] == 'ferret'):
                    make_gen_ferret(stream_path, version_id, app_id, ppi_id)
                else:
                    make_gen(stream_path, version_id, app_id, ppi_id)
                sys.exit()

    print ("Version \"" + args.version_id + "\" not found!")
    registry.close()
    sys.exit()

def edit_json_func(stream_path, args):
    #TODO
    #diferenciar flags da PPI e as do compilador (problema com spar-ferret)
    registry = open(stream_path + '/sys/registry.data', 'r')
    for line in registry:
        if isNotBlank(line):
            aux = line.split()
            if aux[0] == args.version_id:
                cmd_line = args.user_editor + " " + stream_path + "/apps/" + aux[1] + "/" + aux[2] + "/" + args.version_id + "/config.json"
                print("Running >> " + cmd_line)
                os.system(cmd_line)

                #if there is not a Makefile, build one
                #make_path = stream_path + "/apps/" + aux[1] + "/" + aux[2] + "/" + args.version_id + "/Makefile"
                #if os.path.exists(make_path) == False:
                if(aux[1] == 'ferret'):
                    make_gen_ferret(stream_path, aux[0], aux[1], aux[2])
                else:
                    make_gen(stream_path, aux[0], aux[1], aux[2])
                sys.exit()
    registry.close()
    print ("Version \"" + args.version_id + "\" not found!")
    sys.exit()

def compile_func(stream_path, args):

    #TODO
    #adicionar opcao de limitar o 'compile all' por app ou ppi
    #solve the ferret nedd for make clean
    
    # build all makefiles
    register_func(stream_path)
    
    # succeed flag for later verifications
    compilation_succeed = False

    print('Compiling ' + args.version_id + '...')
    registry = open(stream_path + '/sys/registry.data', 'r')
    for line in registry:
        if isNotBlank(line):
            aux = line.split()
            version_id = aux[0]
            app_id = aux[1]
            ppi_id = aux[2]

            # search for a programm name or run for them all
            if (version_id == args.version_id) or (args.version_id.lower() == 'all'):
                bin_path = stream_path + "/bin/" + app_id + "/" + ppi_id
                if os.path.isdir(bin_path) == False:     #if there is not a /bin directory, create one
                    os.makedirs(bin_path)

                #path where make will run
                programm_path = stream_path + "/apps/" + app_id + "/" + ppi_id + "/" + version_id + "/" 

                # check if exists the make directory
                if os.path.isdir(programm_path) == False:
                    print("Error!!\n-> Directory [" + programm_path + "] not found.\n-> Check if it exists or try to register it again!")
                    sys.exit()

                # temporary (or not) solution to solve ferret compiling problem
                bin_file = stream_path + '/bin/' + app_id + "/" + ppi_id + "/" + version_id
                if os.path.exists(bin_file):
                    os.system("make -C " + programm_path + " clean")

                # compile the programm
                if(os.WEXITSTATUS(os.system("make -C " + programm_path))):
                    print("\n ---------------------------------------")
                    print(" Compilation failed!")
                    print(" Version: " + version_id)
                    print("\n Common problems:")
                    print(" - Required library not installed.")
                    print("   See libs/README.md and ppis/README.md \n   and make sure everything is setup.\n")
                    print(" - Required library is installed but not loaded.")
                    print("   Run \'source setup_vars.sh\'\n")
                    print(" - Missing or wrong compiling arguments.")
                    print("   You can use the command \'configure\' \n   to check and modify compiling dependencies.")
                    print("   Run: ./spbench configure -version " + version_id)
                    #else:
                    #    print("\n---------------------------------------")
                    #    print(" Compilation succeed!")
                    #    print(" Version: " + version_id)
                    print(" ---------------------------------------\n")
                    
                compilation_succeed = True

                # if it is compiling a single version, ends here
                if (args.version_id.lower() != 'all'):
                    registry.close()
                    sys.exit()

    registry.close()

    if not compilation_succeed:
        print("Compilation failed. \'" + args.version_id + "\' not found!!")

    sys.exit()

def execute_func(stream_path, args):

    registry = open(stream_path + '/sys/registry.data', 'r')
    version_found=False
    for line in registry:
        if isNotBlank(line):
            aux = line.split()
            version_id = aux[0]
            app_id = aux[1]
            ppi_id = aux[2]
            if (version_id == args.version_id) or (args.version_id.lower() == 'all'):
                version_found=True
                #workloads
                input = args.input_file.replace('$BENCH_DIR', stream_path)

                json_file = stream_path + "/sys/inputs_registry.json"

                #it first look for the input on the registry
                #check if the registry exists
                if os.path.exists(json_file) == True:

                    # read the content of the json registry and store into a dictionary
                    inputs_json = open(json_file, 'r')
                    with inputs_json as f:
                        json_data = json.load(f)
                    inputs_json.close()

                    # get all inputs for the selected application
                    app_inputs = json_data[app_id]
                    for key in app_inputs:
                        if(args.input_file == key): #search for the selected input
                            input = app_inputs[key].replace('$BENCH_DIR', stream_path)
                            break
                else:
                    # if no input found on the registry, it will try to run with the user's custom input
                    print(" Input not found on the registry\n Trying to run with " + input_file + "\n")
                    
                if app_id == 'ferret':
                    input_file = input # ferret is not easy to check the input, so forget it
                else:
                    if os.path.exists(input) == False: # check if the input file exists only for bzip, lane, and person
                        print(" Input file " + input + " not found!")
                        sys.exit()

                if (app_id == 'bzip2') or (app_id == 'ferret'):
                    input_file = " " + os.path.abspath(input)    #generate an input argument for bzip2 or ferret
                else:
                    input_file = " -i " + os.path.abspath(input)    #generate a '-i input_file' argument
                
                #other args
                other_args=''
                if app_id == 'bzip2':
                    num_threads = " -t" + args.nthreads
                    batch = " -b" + args.batch_size
                    time_interval = ''
                    other_args = ' --force --keep'
                    if args.time_interval:
                        time_interval = " -m" + args.time_interval

                    items_reading_frequency = ''
                    if args.items_reading_frequency:
                        items_reading_frequency = " -F" + args.items_reading_frequency
                else:
                    num_threads = " -t " + args.nthreads
                    batch = " -b " + args.batch_size

                    time_interval = ''
                    if args.time_interval:
                        time_interval = " -m " + args.time_interval

                    items_reading_frequency = ''
                    if args.items_reading_frequency:
                        items_reading_frequency = " -F " + args.items_reading_frequency
                    
                exec_arguments = " "
                if args.exec_arguments:
                    exec_arguments = exec_arguments.join(args.exec_arguments)    #join the input with the remaining arguments
                exec_arguments = input_file + num_threads + batch + time_interval + items_reading_frequency + " " + exec_arguments + other_args

                cmd_line = stream_path + "/bin/" + app_id + "/" + ppi_id + "/" + version_id + exec_arguments
                print("Running >> " + cmd_line)
                os.system(cmd_line)
                

    if not version_found:
        print("\n Version " + args.version_id + " not found!\n")
        print(" You can run \'./spbench list\' to see all available versions.")
        print(" Or you can add this version through the command 'new'.")
        print(" Run \'./spbench new -help\' for more info.\n")

    registry.close()        

#print all registry
def list_reg_func(stream_path):

    registry = open(stream_path + '/sys/registry.data', 'r')
    print(" -------------------------------------------------------------------")
    print( color.BOLD +"     APPLICATION               PPI              VERSION NAME       "+color.END)
    print(" -------------------------------------------------------------------")
    for line in registry:
        if isNotBlank(line):
            aux = line.split()
            print(" " + aux[1].ljust(20) + "| " + aux[2].ljust(20) + "| " + aux[0])
    print(" -------------------------------------------------------------------")
    registry.close()
    sys.exit()

#print all inputs
def list_inputs_func(stream_path, args):

    json_file = stream_path + "/sys/inputs_registry.json"
    #check if the registry exists
    if os.path.exists(json_file) == False:
        print("\n There is no inputs registered!\n")
        sys.exit()

    registry = open(json_file, 'r')
    print(" -----------------------------------")
    print(color.BOLD +"   Input ID\tRegistered input       "+color.END)
    print(" -----------------------------------")

    # read the content of the json registry and store into a dictionary
    inputs_json = open(json_file, 'r')
    with inputs_json as f:
        json_data = json.load(f)
    inputs_json.close()

    # print all inputs for the selected application
    if(args.app_id):
        print("\n-> " + args.app_id)
        app_inputs = json_data[args.app_id]
        for key in app_inputs:
            print("   |" + key + "\t" + app_inputs[key])

    else: # print all inputs for all applications
        apps_list = {'bzip2', 'ferret', 'lane_detection'}#, 'person_recognition'}
        for app in apps_list:
            print("\n-> " + app)
            app_inputs = json_data[app]
            for key in app_inputs:
                print("   |" + key + "\t" + app_inputs[key])
    print("")
    registry.close()
    sys.exit()

#delete a single registry entry
def delete_reg_func(stream_path, args):

#TODO
#    "Ps: Falta diferenciar:"
#    "- Remover tudo (sources, diretorios, binarios, outputs e retirada do registro)"
#    "- Remover apenas binario e outputs" (clean)
#    "- Alterar o registro (renomear)"
#    "- Bloquear o delete da versao sequencial"

    register_found = False
    if os.path.exists('sys/registry.data'):
        new_file = open('sys/registry.tmp', 'w')
        #new_file.write(new_register)
        #register_exists = False
        old_file = open('sys/registry.data', 'r')
        for line in old_file:
            if isNotBlank(line):
                aux = line.split()
                if aux[0] != args.version_id: #write back all non matching lines
                    new_file.write(line)
                else: 
                    #version_seq_id = aux[1] + '_sequential' #get the name of the sequential version
                    #print version_seq_id
                    #if aux[0] == version_seq_id: #prevent user from deleting the original sequential version
                    #    print('Error!! You can not delete this sequential version.')
                    #    new_file.write(line)
                    #else:

                    #remove the respective directory and files
                    rm_app = "rm -r " + stream_path + "/apps/" + aux[1] + "/" + aux[2] + "/" + args.version_id
                    os.system(rm_app)

                    #if there is no other version related to this library, remove also the library directory
                    lib_path = stream_path + "/apps/" + aux[1] + "/" + aux[2]
                    directory = os.listdir(lib_path) 
                    if len(directory) == 0: 
                        os.system("rm -r " + lib_path)

                    #remove the binary
                    bin_path = stream_path + "/bin/" + aux[1] + "/" + aux[2] + "/" + args.version_id
                    if os.path.exists(bin_path):
                        os.system("rm " + bin_path)

                    register_found = True

        #Replace the old registry with the new one
        os.remove('sys/registry.data')
        os.rename('sys/registry.tmp', 'sys/registry.data')
        if register_found:
            print("\n " + args.version_id + " deleted!\n")
        else:
            print("\n " + args.version_id + " not found!\n")
            print(" Run \'./spbench list\' and make sure this version is on the list.\n")
    else:
        print(" No registry file found!")
    new_file.close()
    old_file.close()


#add a new input to the suite
def new_input_func(stream_path, args):

    json_file = stream_path + "/sys/inputs_registry.json"
    
    #check if the registry exists
    if os.path.exists(json_file) == False:
        # if there is not a json file, create and initialize one
        inputs_registry = {"bzip2":{},"ferret":{},"lane_detection":{}}
        with open(json_file, 'w') as f:
            json.dump(inputs_registry, f)
        f.close()

    # read the content of the json registry and store into a dictionary
    inputs_json = open(json_file, 'r')
    with inputs_json as f:
        json_data = json.load(f)
    inputs_json.close()

    # get all inputs for the selected application
    app_inputs = json_data[args.app_id]
    existent_input = ""
    for key in app_inputs:
        if(args.input_id == key): #search for existent input IDs
            existent_input = app_inputs[key]
            break

    #existent_input.replace('$BENCH_DIR', stream_path)

    if(existent_input):
        print("\n There is already an input named \'"+ args.input_id +"\' for " + args.app_id + ".\n")
        print(" Registered input -> " + existent_input.replace('$BENCH_DIR', stream_path))

        # user input support for python 2 and 3
        if(python_3 == 3):
            answer = input("\n Do you want to replace it? [yes/no]\n")
        else:
            answer = raw_input("\n Do you want to replace it? [yes/no]\n")

        if(answer.lower() not in ["y","yes"]):
            print(" Your input was not registered!\n")
            sys.exit()

    #update the dictionary with the new entry
    json_data[args.app_id].update({args.input_id:args.input})

    # write the dictionary to the json registry file
    print("\n\t New input added!\n")
    print(" Application: " + args.app_id)
    print("    Input ID: " + args.input_id)
    print("       Input: " + args.input + "\n")
    with open(json_file, 'w') as f:
        json.dump(json_data, f, indent=4)
    f.close()
    sys.exit()
