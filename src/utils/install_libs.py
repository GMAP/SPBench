import os
import sys
from src.utils.dict import *

def prompt_users():
    print("Please select an option:")
    print(" 1) Proceed with the installation of the next libraries (WARNING: This may cause errors if the libraries depend on this one!).")
    print(" 2) Stop the libraries installation.")
    choice = input("Enter your choice: ")
    if choice == '1':
        print("Trying to install the remaining libraries...")
    elif choice == '2':
        print("Exiting the installation...")
        sys.exit(1)
    else:
        print("\n ERROR: INVALID CHOICE!\n")
        return 1
    return 0

def install_libraries(spbench_path, app_id):

    dependency_dict = getDependenciesRegistry(spbench_path)

    # Takes the app_id dependencies from the dictionary
    app_dependecies = dependency_dict[app_id]

    # Initialize a dependencies list with the length of the dependencies
    dependencies_list = [None] * len(app_dependecies)

    # Fill the dependencies list with the dependencies in the correct order in the list
    for key, val in app_dependecies.items():
        dependencies_list[int(key)-1] = val

    # Store the original directoryctory
    libs_dir = os.path.join(spbench_path, "libs")

    # Iterate over the dependencies and attempt to install each one
    for dependency in dependencies_list:        
        print("---------------------------------------")
        print(f" Installing {dependency}...")
        print("---------------------------------------")
        dependency_dir = os.path.join(libs_dir, dependency)
        try:
            setup_script = os.path.join(dependency_dir, f"setup_{dependency}.sh")
            if os.system(f". {setup_script} {dependency_dir}") != 0:
                print("*************** ERROR *****************")
                print("")
                print(f" SPBench failed to install {dependency}. Please check the error messages above.")
                print("")
                print(f" The {dependency} library can be found at {os.path.join(libs_dir, dependency)}. You can try to install it manually.")
                print("")
                print("***************************************")
                if prompt_users() != 0:
                    os.chdir(libs_dir)
                    sys.exit(1)
        except Exception as e:
            print(f"*************** ERROR *****************")
            print("")
            print(f" SPBench failed to install {dependency}. Please check the error messages above.")
            print("")
            print(f" The {dependency} library can be found at {os.path.join(libs_dir, dependency)}. You can try to install it manually.")
            print("")
            print("***************************************")
            if prompt_users() != 0:
                os.chdir(libs_dir)
                sys.exit(1)
        
        #vars_script = os.path.join(libs_dir, dependency, f"setup_{dependency}_vars.sh")
        #exec(open(vars_script).read())
        os.chdir(libs_dir)
        print("DONE!")