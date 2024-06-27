import os
import sys
import logging
import urllib.request
import tarfile

THIS_SCRIPT = "setup_upl.sh"
LIB_NAME = "UPL"

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

def progress_hook(blocknum, blocksize, totalsize):
    """Function to display download progress."""
    percent = int(blocknum * blocksize * 100 / totalsize)
    sys.stdout.write(f"\rDownloading {LIB_NAME}: {percent}%")
    sys.stdout.flush()

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

def main():

    global THIS_SCRIPT
    global THIS_DIR
    global LIB_FILE_PATH
    global LIB_NAME
    global FILE_URL

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

    LIB_FILE_PATH = os.path.join(THIS_DIR, "upl.tar.xz")
    FILE_URL = "https://gmap.pucrs.br/public_data/spbench/libs/upl/upl.tar.xz"

    if not os.path.isfile(LIB_FILE_PATH):
        download_file()

    try:
        with tarfile.open(LIB_FILE_PATH, 'r:xz') as tar:
            tar.extractall()
    except Exception as e:
        logging.error(f"Failed to extract {LIB_FILE_PATH}: {e}")
        logging.info("Trying to download it again...")
        download_file(FILE_URL, LIB_FILE_PATH)
        try:
            with tarfile.open(LIB_FILE_PATH, 'r:xz') as tar:
                tar.extractall()
        except Exception as e:
            logging.error(f"Failed to extract {LIB_FILE_PATH} again: {e}")
            return 1

    return 0

if __name__ == "__main__":
    setup_logging()
    main()