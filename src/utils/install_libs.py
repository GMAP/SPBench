import os
import sys
import subprocess

import tarfile
import logging
import shutil
import urllib.request
import runpy

from src.utils.dict import *
from src.utils.utils import *

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

# def progress_hook(blocknum, blocksize, totalsize):
#     """Function to display download progress."""
#     percent = int(blocknum * blocksize * 100 / totalsize)
#     sys.stdout.write(f"\rDownload progress: {percent}%")
#     sys.stdout.flush()

# def get_library_data(lib_data_file):
#     # Get the registry for the dependency
#     lib_data_dict = getDictFromJSON(lib_data_file)

#     try:
#         library_name = lib_data_dict["LIB_NAME"]
#         library_file = lib_data_dict["LIB_FILE"]
#         library_repo = (f"{lib_data_dict["FILE_REPO"]}/{library_file}")

#     except KeyError as e:
#         logging.error(f"KeyError: {e}")
#         logging.error(f"Error while getting the library data.")
#         logging.error(f"Please check the lib_data.json file and try again.")
#         if prompt_users() != 0:
#             os.chdir(SPBENCH_LIBS_PATH)
#             sys.exit(1)

#     logging.info(f"Library name: {library_name}")
#     logging.info(f"Library file: {library_file}")
#     logging.info(f"Library repo: {library_repo}")

#     return library_name, library_file, library_repo

# def download_file():
#     try:
#         original_dir = os.getcwd()  # Store original directory
#         os.chdir(DEPENDENCY_PATH)        
#         if os.path.isfile(LIB_FILE):
#             logging.info(f"{LIB_FILE} already exists at {DEPENDENCY_PATH}.")
#             return
        
#         logging.info(f"Downloading {LIB_NAME} from {LIB_REPO}...")
#         urllib.request.urlretrieve(LIB_REPO, DEPENDENCY_PATH, reporthook=progress_hook)
#         logging.info(f"Download of {LIB_NAME} completed.")
    
#     except urllib.error.URLError as e:
#         logging.error(f"Failed to download {LIB_NAME}: {e}")
#         sys.exit(1)
    
#     except Exception as e:
#         logging.error(f"Error during download process: {e}")
#         sys.exit(1)
    
#     finally:
#         os.chdir(original_dir)  # Return to original directory

# def remove_directory(dir_path):
#     try:
#         logging.info(f"Removing directory: {dir_path}")
#         shutil.rmtree(dir_path)
#         logging.info(f"Successfully removed directory: {dir_path}")
#     except Exception as e:
#         logging.error(f"Error removing directory {dir_path}: {e}")
#         sys.exit(1)

# def libfile_exists():
#     try:
#         original_dir = os.getcwd()  # Store original directory
#         os.chdir(DEPENDENCY_PATH)
        
#         if not os.path.isfile(LIB_FILE):
#             logging.info(f"The library file {LIB_FILE} was not found. Trying to download it first...")
#             download_file()
            
#             # Check again after attempting download
#             if not os.path.isfile(LIB_FILE):
#                 logging.error(f"Failed to download {LIB_FILE}.")
#                 return False
        
#         logging.info(f"Found library file {LIB_FILE} at {DEPENDENCY_PATH}")
#         return True
    
#     except Exception as e:
#         logging.error(f"Error checking or downloading library file: {e}")
#         return False
    
#     finally:
#         os.chdir(original_dir)  # Return to original directory

# def extract_files(num_tries = 3):
#     try:
#         original_dir = os.getcwd()  # Store original directory
#         os.chdir(DEPENDENCY_PATH)
        
#         while num_tries > 0:
#             if not libfile_exists():
#                 num_tries -= 1
#                 logging.info(f"Attempt {4 - num_tries}: Library file {LIB_FILE} not found.")
#                 if num_tries == 0:
#                     logging.error("Failed to install the library after all attempts.")
#                     sys.exit(1)
#                 logging.info(f"Trying to download {LIB_FILE} again...")
#                 download_file()
#             else:
#                 logging.info(f"Library file {LIB_FILE} found. Attempting to decompress it...")
#                 try:
#                     # Determine the compression type based on the file extension
#                     if LIB_FILE_PATH.endswith('.tar.gz'):
#                         mode = 'r:gz'
#                     elif LIB_FILE_PATH.endswith('.tar.bz2'):
#                         mode = 'r:bz2'
#                     elif LIB_FILE_PATH.endswith('.tar.gz'):
#                         mode = 'r:gz'
#                     else:
#                         mode = 'r'
#                     with tarfile.open(LIB_FILE_PATH, mode) as tar:
#                         for member in tar.getmembers():
#                             logging.info(f"Extracting {member.name}...")
#                             tar.extract(member, path=LIB_PATH)

#                     logging.info(f"Decompression of {LIB_FILE} was successful.")
#                     return
#                 except tarfile.TarError:
#                     logging.error(f"Decompression of {LIB_FILE} failed.")
#                     num_tries -= 1
#                     if num_tries > 0:
#                         logging.info(f"Retrying... {num_tries} tries left.")
#                     else:
#                         logging.error("Failed to install the library after all attempts.")
#                         sys.exit(1)
    
#     except Exception as e:
#         logging.error(f"Error during extraction process: {e}")
#         sys.exit(1)
    
#     finally:
#         os.chdir(original_dir)  # Return to original directory

# Function to prompt the user for input
# def prompt_user():
#     print("Please select an option:")
#     print(" 1) Do not reinstall (keep it as it is)")
#     print(" 2) Download the library again, decompress the files, configure, build and install")
#     print(" 3) Decompress already downloaded files, configure, build, and install")
#     print(" 4) Only configure, build and install")
#     choice = input("Enter your choice (1-4): ")
#     if choice == "1":
#         logging.info("Keeping the current installation.")
#     elif choice == "2":
#         logging.info("Removing the current installation...")
#         remove_directory()
#         download_file()
#         extract_files()
#         configure_library()
#         build_library()
#         if not install_library():
#             return False
#     elif choice == "3":
#         logging.info("Removing the current installation...")
#         remove_directory(LIB_PATH)
#         extract_files()
#         configure_library()
#         build_library()
#         if not install_library():
#             return False
#     elif choice == "4":
#         configure_library()
#         build_library()
#         if not install_library():
#             return False
#     else:
#         logging.error("\nERROR: INVALID CHOICE!\n")
#         if not prompt_user():
#             return False
#     return True

def install_libraries(spbench_path, app_id):
    # Initialize logging if needed
    # logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

    # setup_logging()

    dependency_dict = getDependenciesRegistry(spbench_path)

    # Takes the app_id dependencies from the dictionary
    app_dependencies = dependency_dict[app_id]

    # Initialize a dependencies list with the length of the dependencies
    dependencies_list = [None] * len(app_dependencies)

    # Fill the dependencies list with the dependencies in the correct order in the list
    for key, val in app_dependencies.items():
        dependencies_list[int(key) - 1] = val

    global SPBENCH_LIBS_PATH
    # Store the original directory
    SPBENCH_LIBS_PATH = os.path.join(spbench_path, "libs")

    print("DEPENDENCIES LIST: ", dependencies_list)

    # If opencv is in the dependencies list, check if cmake is available in the system
    if "opencv" in dependencies_list:
        if shutil.which("cmake") is None:
            logging.error("CMake is required to install OpenCV.")
            logging.error("Please install CMake and try again.")
            if prompt_users() != 0:
                os.chdir(SPBENCH_LIBS_PATH)
                sys.exit(1)

    for dependency in dependencies_list:

        print("---------------------------------------")
        print(f" Installing {dependency}...")
        print("---------------------------------------")

        dependency_dir = os.path.join(SPBENCH_LIBS_PATH, dependency)
        setup_script = os.path.join(dependency_dir, f"setup_{dependency}.py")
        lib_data_file = os.path.join(dependency_dir, "lib_data.json")


        logging.info(f"Preparing {dependency}...")
        logging.info(f"Running > {sys.executable} {setup_script} {dependency_dir}")

        logging.info(f"Getting {dependency} data...")

        # if not os.path.exists(lib_data_file):
        #     logging.error(f"lib_data.json file not found for {dependency}.")
        #     logging.error(f"Please check if the file exists and try again.")
        #     if prompt_users() != 0:
        #         os.chdir(SPBENCH_LIBS_PATH)
        #         sys.exit(1)

        # Get and set the library data 
        # global LIB_NAME, LIB_FILE, LIB_REPO, LIB_FILE_PATH, DEPENDENCY_PATH, LIB_PATH
        # LIB_NAME, LIB_FILE, LIB_REPO = get_library_data(lib_data_file)
        # DEPENDENCY_PATH = os.path.join(SPBENCH_LIBS_PATH, dependency) # Path to the dependency directory
        # LIB_PATH = os.path.join(DEPENDENCY_PATH, LIB_NAME) # Path to the extracted library directory
        # LIB_FILE_PATH = os.path.join(DEPENDENCY_PATH, LIB_FILE) # Path to the compressed library file

        try:
            # Change to the dependency directory
            os.chdir(dependency_dir)

            # Save the original sys.argv
            original_argv = sys.argv

            # Set sys.argv to the required arguments for the script
            sys.argv = [setup_script, dependency_dir]

            # Run the setup script as a module
            runpy.run_path(setup_script, run_name="__main__")

            # Restore the original sys.argv
            sys.argv = original_argv

            # Change back to the original directory
            os.chdir(SPBENCH_LIBS_PATH)

        except Exception as e:
            logging.exception(f"Exception occurred while installing {dependency}.")
            if prompt_users() != 0:
                os.chdir(SPBENCH_LIBS_PATH)
                sys.exit(1)

# def install_libraries(spbench_path, app_id):

#     dependency_dict = getDependenciesRegistry(spbench_path)

#     # Takes the app_id dependencies from the dictionary
#     app_dependecies = dependency_dict[app_id]

#     # Initialize a dependencies list with the length of the dependencies
#     dependencies_list = [None] * len(app_dependecies)

#     # Fill the dependencies list with the dependencies in the correct order in the list
#     for key, val in app_dependecies.items():
#         dependencies_list[int(key)-1] = val

#     # Store the original directoryctory
#     libs_dir = os.path.join(spbench_path, "libs")

#     # Iterate over the dependencies and attempt to install each one
#     for dependency in dependencies_list:        
#         print("---------------------------------------")
#         print(f" Installing {dependency}...")
#         print("---------------------------------------")
#         dependency_dir = os.path.join(libs_dir, dependency)
#         try:
#             setup_script = os.path.join(dependency_dir, f"setup_{dependency}.py")
#             print(f"Running > python {setup_script} {dependency_dir}")
#             if os.system(f"python {setup_script} {dependency_dir}") != 0:
#                 print("*************** ERROR *****************")
#                 print("")
#                 print(f" SPBench failed to install {dependency}. Please check the error messages above.")
#                 print("")
#                 print(f" The {dependency} library can be found at {dependency_dir}. You can try to install it manually.")
#                 print("")
#                 print("***************************************")
#                 if prompt_users() != 0:
#                     os.chdir(libs_dir)
#                     sys.exit(1)
#         except Exception as e:
#             print(f"*************** ERROR *****************")
#             print("")
#             print(f" SPBench failed to install {dependency}. Please check the error messages above.")
#             print("")
#             print(f" The {dependency} library can be found at {dependency_dir}. You can try to install it manually.")
#             print("")
#             print("***************************************")
#             if prompt_users() != 0:
#                 os.chdir(libs_dir)
#                 sys.exit(1)
        
        #vars_script = os.path.join(libs_dir, dependency, f"setup_{dependency}_vars.sh")
        #exec(open(vars_script).read())
        os.chdir(SPBENCH_LIBS_PATH)
        print("DONE!")