#!/usr/bin/env python3

import os
import sys
import subprocess
import tarfile
import logging
import shutil
import urllib.request

THIS_SCRIPT = "setup_ffmpeg.sh"

LIB_FILE = "ffmpeg-3.4.8.tar.gz"
LIB_NAME = "ffmpeg-3.4.8"
HOST_URL = "https://gmap.pucrs.br"
FILE_URL = f"{HOST_URL}/public_data/spbench/libs/ffmpeg/{LIB_FILE}"
num_tries = 3

def setup_logging():
    """Configure logging for the script."""
    logging.basicConfig(
        level=logging.INFO,  # Set logging level to INFO
        format='%(asctime)s - %(levelname)s - %(message)s',  # Define log message format
        datefmt='%Y-%m-%d %H:%M:%S',  # Define date format
        handlers=[logging.StreamHandler()]  # Output logs to console
    )
    
def get_script_dir():
    """Function to determine the directory of the currently executed or sourced script."""
    script = os.path.realpath(sys.argv[0])
    script_dir = os.path.dirname(script)
    return script_dir

def check_script_exists(script_dir, script_name):
    """Function to check if the specified script exists in the given directory."""
    script_path = os.path.join(script_dir, script_name)
    if not os.path.isfile(script_path):
        logging.error(f"The script {script_path} was not found.")
        logging.error("Please make sure you are running this script from the same directory where the script is, or pass this script path as an argument.")
        sys.exit(1)
    return script_path

def download_file():
    try:
        original_dir = os.getcwd()  # Store original directory
        os.chdir(THIS_DIR)        
        if os.path.isfile(LIB_FILE_PATH):
            logging.info(f"{LIB_NAME} already exists at {LIB_FILE_PATH}.")
            return
        
        logging.info(f"Downloading {LIB_NAME} from {FILE_URL}...")
        urllib.request.urlretrieve(FILE_URL, LIB_FILE_PATH, reporthook=progress_hook)
        logging.info(f"Download of {LIB_NAME} completed.")
    
    except urllib.error.URLError as e:
        logging.error(f"Failed to download {LIB_NAME}: {e}")
        sys.exit(1)
    
    except Exception as e:
        logging.error(f"Error during download process: {e}")
        sys.exit(1)
    
    finally:
        os.chdir(original_dir)  # Return to original directory

def progress_hook(blocknum, blocksize, totalsize):
    """Function to display download progress."""
    percent = int(blocknum * blocksize * 100 / totalsize)
    sys.stdout.write(f"\rDownloading {LIB_NAME}: {percent}%")
    sys.stdout.flush()

def libfile_exists():
    try:
        original_dir = os.getcwd()  # Store original directory
        os.chdir(THIS_DIR)
        
        if not os.path.isfile(LIB_FILE_PATH):
            logging.info(f"The library file {LIB_FILE} was not found. Trying to download it first...")
            download_file()
            
            # Check again after attempting download
            if not os.path.isfile(LIB_FILE_PATH):
                logging.error(f"Failed to download {LIB_FILE}.")
                return False
        
        logging.info(f"Found library file {LIB_FILE} at {LIB_FILE_PATH}")
        return True
    
    except Exception as e:
        logging.error(f"Error checking or downloading library file: {e}")
        return False
    
    finally:
        os.chdir(original_dir)  # Return to original directory

def extract_files():
    try:
        original_dir = os.getcwd()  # Store original directory
        os.chdir(THIS_DIR)
        
        global num_tries
        
        while num_tries > 0:
            if not libfile_exists():
                num_tries -= 1
                logging.info(f"Attempt {4 - num_tries}: Library file {LIB_FILE} not found.")
                if num_tries == 0:
                    logging.error("Failed to install the library after all attempts.")
                    sys.exit(1)
                logging.info(f"Trying to download {LIB_FILE} again...")
                download_file()
            else:
                logging.info(f"Library file {LIB_FILE} found. Attempting to decompress it...")
                try:
                    with tarfile.open(LIB_FILE_PATH, 'r:xz') as tar:
                        for member in tar.getmembers():
                            logging.info(f"Extracting {member.name}...")
                            tar.extract(member, path=THIS_DIR)
                    logging.info(f"Decompression of {LIB_FILE} was successful.")
                    return
                except tarfile.TarError:
                    logging.error(f"Decompression of {LIB_FILE} failed.")
                    num_tries -= 1
                    if num_tries > 0:
                        logging.info(f"Retrying... {num_tries} tries left.")
                    else:
                        logging.error("Failed to install the library after all attempts.")
                        sys.exit(1)
    
    except Exception as e:
        logging.error(f"Error during extraction process: {e}")
        sys.exit(1)
    
    finally:
        os.chdir(original_dir)  # Return to original directory

def libdir_exists():
    try:
        original_dir = os.getcwd()  # Store original directory
        os.chdir(THIS_DIR)
        
        # Check if the library directory exists
        if os.path.isdir(LIB_PATH):
            # Check if the directory is not empty
            if not os.listdir(LIB_PATH):
                logging.info(f"The library directory {LIB_PATH} was found but is empty. Trying to get the files...")
                extract_files()
            return True
        
        logging.info(f"The library directory {LIB_PATH} was not found. Trying to get the files...")
        extract_files()
        return True
    
    except Exception as e:
        logging.error(f"Error checking or accessing library directory: {e}")
        return False
    
    finally:
        os.chdir(original_dir)  # Return to original directory

# Function to build the libraryrary
def configure_library():
    if libdir_exists():
        original_dir = os.getcwd()
        os.chdir(LIB_PATH)
        
        # Check if the library has a configure file
        if not os.path.isfile("configure"):
            logging.info("The library does not have a configure file. Trying to run the previous steps...")
            extract_files()
        
        # Check if the build directory exists
        build_dir = os.path.join(LIB_PATH, "build")
        if not os.path.isdir(build_dir):
            logging.info("The build directory does not exist. Creating it...")
            os.mkdir(build_dir)
        
        os.chdir(LIB_PATH)
        PREFIX = os.path.join(os.getcwd(), "build")
        os.environ["PATH"] = f"{PREFIX}/bin:{os.environ['PATH']}"
        os.environ["PKG_CONFIG_PATH"] = f"{PREFIX}/pkgconfig"
        logging.info("Configuring the library...")
        
        try:
            result = subprocess.run(["./configure", f"--prefix={PREFIX}", "--enable-shared", " --enable-nonfree", "--enable-pic"], check=True, capture_output=True, text=True)
            logging.info(result.stdout)
            if result.stderr:
                logging.error(result.stderr)
        except subprocess.CalledProcessError as e:
            logging.error(f"Configuration failed with error: {e}")
        finally:
            os.chdir(original_dir)

def build_library():
    if libdir_exists():
        original_dir = os.getcwd()
        os.chdir(LIB_PATH)
        logging.info("Building the library...")
        
        # Check if there is a Makefile
        if not any(f.startswith("Makefile") for f in os.listdir(LIB_PATH)):
            logging.info("Makefile not found. Trying to run the previous steps...")
            extract_files()
            configure_library()
        
        try:
            result = subprocess.run(["make", "-j"], check=True, capture_output=True, text=True)
            logging.info(result.stdout)
            if result.stderr:
                logging.error(result.stderr)
            logging.info("The library was built successfully.")
        except subprocess.CalledProcessError as e:
            logging.error(f"Failed to build the library: {e}")
        finally:
            os.chdir(original_dir)

# Function to install the library
def install_library():
    if libdir_exists():
        os.chdir(LIB_PATH)
        # Check if the library has been built
        if not os.path.isdir("build"):
            print("The library has not been built yet. Building it first...")
            build_library()
        os.chdir(LIB_PATH)
        print("Installing the library...")
        result = subprocess.run(["make", "install", "-j"])
        if result.returncode == 0:
            print("The library was installed successfully.")
            return True
        else:
            print("Failed to install the library.")
            return False

def remove_directory(dir_path):
    try:
        logging.info(f"Removing directory: {dir_path}")
        shutil.rmtree(dir_path)
        logging.info(f"Successfully removed directory: {dir_path}")
    except Exception as e:
        logging.error(f"Error removing directory {dir_path}: {e}")
        sys.exit(1)

# Function to prompt the user for input
def prompt_user():
    print("Please select an option:")
    print(" 1) Do not reinstall (keep it as it is)")
    print(" 2) Download the library again, decompress the files, configure, build and install")
    print(" 3) Decompress the downloaded files, configure, build, and install")
    print(" 4) Only configure, build and install")
    choice = input("Enter your choice (1-4): ")
    if choice == "1":
        logging.info("Keeping the current installation.")
    elif choice == "2":
        logging.info("Removing the current installation...")
        remove_directory(LIB_PATH)
        download_file()
        extract_files()
        configure_library()
        build_library()
        if not install_library():
            return False
    elif choice == "3":
        logging.info("Removing the current installation...")
        remove_directory(LIB_PATH)
        extract_files()
        configure_library()
        build_library()
        if not install_library():
            return False
    elif choice == "4":
        configure_library()
        build_library()
        if not install_library():
            return False
    else:
        logging.error("\nERROR: INVALID CHOICE!\n")
        if not prompt_user():
            return False
    return True

def main():

    setup_logging()

    global THIS_DIR
    global LIB_FILE_PATH
    global LIB_PATH

    if len(sys.argv) < 2:
        logging.info("The directory of the currently executed script was not passed as an argument.")
        logging.info("Trying to determine the directory of the currently executed script...")
        THIS_DIR = get_script_dir()
    else:
        THIS_DIR = sys.argv[1]


     # Check if the specified script exists in the provided directory
    script_path = check_script_exists(THIS_DIR, THIS_SCRIPT)
    
    # Proceed with script execution or further operations
    logging.info(f"Script found: {script_path}")

    logging.info(f"Trying to run {os.path.join(THIS_DIR, THIS_SCRIPT)}...")

    LIB_FILE_PATH = os.path.join(THIS_DIR, LIB_FILE)
    LIB_PATH = os.path.join(THIS_DIR, LIB_NAME)

    os.chdir(THIS_DIR)

    # Check if the directory exists
    if os.path.isdir(LIB_PATH):
        logging.info("A previous installation of the library exists.")
        if not prompt_user():
            sys.exit(1)
    else:
        logging.info("No previous installation found. Proceeding with a fresh installation...")
        download_file()
        extract_files()
        configure_library()
        build_library()
        if not install_library():
            sys.exit(1)

    os.chdir(THIS_DIR)

if __name__ == "__main__":
    main()