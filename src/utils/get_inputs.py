## 
 ##############################################################################
 #  File  : get_inputs.py
 #
 #  Title : Script to download and install the applications inputs
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

import os
import subprocess
import sys

PUB_DATA_URL = "https://gmap.pucrs.br/public_data/spbench/workloads"

def download_and_extract(app_id, class_id, tar_file, force):
	"""
	Download and extract a tar file.
      
	Parameters:
	- app_id: str : Application ID.
	- class_id: str : Class ID.
	- tar_file: str : Tar file name.
	- force: bool : Force download and extraction.
	"""
	if force and os.path.isfile(tar_file):
		os.remove(tar_file)
	if not os.path.isfile(tar_file):
		print(f" Downloading {tar_file}...")
		subprocess.run(["wget", f"{PUB_DATA_URL}/{app_id}/{tar_file}"])
	if os.path.isfile(tar_file):
		print(f" Building {app_id} {class_id} class...")
		subprocess.run(["tar", "-xf", tar_file])

def concatenate_file_with_itself(input_file, output_file, chunk_size=1024*1024):
    """
    Concatenate a file with itself by reading and writing in chunks.

    Parameters:
    - input_file: str : Path to the input file to be concatenated with itself.
    - output_file: str : Path to the output file.
    - chunk_size: int : Size of chunks to read and write at a time (default 1 MB).
    """
    with open(output_file, 'wb') as outfile:
        for _ in range(2):  # Loop twice to read the same file two times
            with open(input_file, 'rb') as infile:
                while True:
                    chunk = infile.read(chunk_size)
                    if not chunk:
                        break
                    outfile.write(chunk)
		
def get_inputs_func(spbench_path, app_id, class_id, force):
	"""
	Download, extract, and prepare the inputs for SPBench applications.
      
	Parameters:
	- spbench_path: str : Path to the SPBench directory.
	- app_id: str : Application ID.
	- class_id: str : Class ID.
	- force: bool : Force download and extraction.
	"""
	inputs_dir = spbench_path + "/inputs"
	os.chdir(inputs_dir)

	print("---------------------------------------------------------------")

	if app_id == "lane_detection" or app_id == "all":
		this_app = "lane_detection"
		if not os.path.exists("lane_detection"):
			os.mkdir("lane_detection")
		os.chdir("lane_detection")

		if class_id == "test" or class_id == "all":
			this_class = "test"
			FILE = f"{this_class}.tar.gz"
			download_and_extract(this_app, this_class, FILE, force)
			print("---------------------------------------------------------------")
			return

		if class_id == "small" or class_id == "all":
			this_class = "small"
			FILE = f"{this_class}.tar.gz"
			download_and_extract(this_app, this_class, FILE, force)
			print("---------------------------------------------------------------")

		if class_id == "medium" or class_id == "all":
			this_class = "medium"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			download_and_extract(this_app, this_class, FILE, force)
			print("---------------------------------------------------------------")

		if class_id == "large" or class_id == "all":
			this_class = "large"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			download_and_extract(this_app, this_class, FILE, force)
			print("---------------------------------------------------------------")

		if class_id == "huge" or class_id == "all":
			this_class = "huge"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			download_and_extract(this_app, this_class, FILE, force)
			print("---------------------------------------------------------------")

		os.chdir("..")

	if app_id == "fraud_detection" or app_id == "all":
		this_app = "fraud_detection"
		if not os.path.exists("fraud_detection"):
			os.mkdir("fraud_detection")
		os.chdir("fraud_detection")

		print(" Building Fraud Detection input classes...")

		FILE = "fraud_detection.tar.gz"
		download_and_extract(this_app, "all", FILE, force)
		print("---------------------------------------------------------------")

		os.chdir("..")

	if app_id == "person_recognition" or app_id == "all":
		this_app = "person_recognition"
		if not os.path.exists("person_recognition"):
			os.mkdir("person_recognition")
		os.chdir("person_recognition")

		print(" Building Person Recognition input classes...")

		FILE = "person_inputs.tar.gz"
		download_and_extract(this_app, "all", FILE, force)
		print("---------------------------------------------------------------")

		os.chdir("..")

	if app_id == "ferret" or app_id == "all":
		this_app = "ferret"
		if not os.path.exists("ferret"):
			os.mkdir("ferret")
		os.chdir("ferret")

		if class_id == "test" or class_id == "small" or class_id == "all":
			this_class = "small"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			download_and_extract(this_app, this_class, FILE, force)
			print("---------------------------------------------------------------")

		if class_id == "medium" or class_id == "all":
			this_class = "medium"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			download_and_extract(this_app, this_class, FILE, force)
			print("---------------------------------------------------------------")

		if class_id == "large" or class_id == "all":
			this_class = "large"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			download_and_extract(this_app, this_class, FILE, force)
			print("---------------------------------------------------------------")

		if class_id == "huge" or class_id == "all":
			this_class = "huge"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			download_and_extract(this_app, this_class, FILE, force)
			print("---------------------------------------------------------------")

		os.chdir("..")

	if app_id == "bzip2" or app_id == "all":
		this_app = "bzip2"
		if not os.path.exists("bzip2"):
			os.mkdir("bzip2")
		os.chdir("bzip2")

		if class_id == "test" or class_id == "all":
			this_class = "test"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			download_and_extract(this_app, this_class, FILE, force)
			print("---------------------------------------------------------------")

		if class_id == "small" or class_id == "all":
			this_class = "small"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			download_and_extract(this_app, this_class, FILE, force)
			print("---------------------------------------------------------------")

		if class_id == "medium" or class_id == "all":
			this_class = "medium"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			download_and_extract(this_app, this_class, FILE, force)

		if class_id == "large" or class_id == "all":
			this_class = "large"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			download_and_extract(this_app, this_class, FILE, force)
			print("---------------------------------------------------------------")

		if class_id == "huge" or class_id == "all":
			this_class = "huge"
			FILE = "large.tar.gz"
			download_and_extract(this_app, this_class, FILE, force)
			#if os.path.isfile("enwiki-20211120-pages-articles-multistream9.xml"):
				#subprocess.run(["cp", "enwiki-20211120-pages-articles-multistream9.xml", "enwiki-20211120-pages-articles-multistream9-2x.xml"])
				#subprocess.run(["cp", "enwiki-20211120-pages-articles-multistream9.xml", "-", ">>", "enwiki-20211120-pages-articles-multistream9-2x.xml"])
			if os.path.isfile("enwiki-20211120-pages-articles-multistream9.xml"):
				concatenate_file_with_itself("enwiki-20211120-pages-articles-multistream9.xml", "enwiki-20211120-pages-articles-multistream9-2x.xml")
			print(" Done!")
			print("---------------------------------------------------------------")

		os.chdir(spbench_path)