import os
import subprocess
import sys

def get_inputs_func(spbench_path, app_id, class_id, force):

	inputs_dir = spbench_path + "/inputs"
	os.chdir(inputs_dir)

	PUB_DATA_URL = "https://gmap.pucrs.br/public_data/spbench/workloads"

	print("---------------------------------------------------------------")

	if app_id == "lane_detection" or app_id == "all":
		this_app = "lane_detection"
		if not os.path.exists("lane_detection"):
			os.mkdir("lane_detection")
		os.chdir("lane_detection")

		if class_id == "test" or class_id == "all":
			this_class = "test"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			if not os.path.isfile(FILE):
				print(f" Downloading {FILE}...")
				subprocess.run(["wget", f"{PUB_DATA_URL}/{this_app}/{FILE}"])
			print(f" Building {this_app} {this_class} class...")
			if os.path.isfile(FILE):
				subprocess.run(["tar", "-xf", FILE])
			else:
				print(f" File {FILE} not found!")
			print(" Done!")
			print("---------------------------------------------------------------")

		if class_id == "small" or class_id == "all":
			this_class = "small"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			if not os.path.isfile(FILE):
				print(f" Downloading {FILE}...")
				subprocess.run(["wget", f"{PUB_DATA_URL}/{this_app}/{FILE}"])
			print(f" Building {this_app} {this_class} class...")
			if os.path.isfile(FILE):
				subprocess.run(["tar", "-xf", FILE])
			else:
				print(f" File {FILE} not found!")
			print(" Done!")
			print("---------------------------------------------------------------")

		if class_id == "medium" or class_id == "all":
			this_class = "medium"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			if not os.path.isfile(FILE):
				print(f" Downloading {FILE}...")
				subprocess.run(["wget", f"{PUB_DATA_URL}/{this_app}/{FILE}"])
			print(f" Building {this_app} {this_class} class...")
			if os.path.isfile(FILE):
				subprocess.run(["tar", "-xf", FILE])
			else:
				print(f" File {FILE} not found!")
			print(" Done!")
			print("---------------------------------------------------------------")

		if class_id == "large" or class_id == "all":
			this_class = "large"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			if not os.path.isfile(FILE):
				print(f" Downloading {FILE}...")
				subprocess.run(["wget", f"{PUB_DATA_URL}/{this_app}/{FILE}"])
			print(f" Building {this_app} {this_class} class...")
			if os.path.isfile(FILE):
				subprocess.run(["tar", "-xf", FILE])
			else:
				print(f" File {FILE} not found!")
			print(" Done!")
			print("---------------------------------------------------------------")

		if class_id == "huge" or class_id == "all":
			this_class = "huge"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			if not os.path.isfile(FILE):
				print(f" Downloading {FILE}...")
				subprocess.run(["wget", f"{PUB_DATA_URL}/{this_app}/{FILE}"])
			print(f" Building {this_app} {this_class} class...")
			if os.path.isfile(FILE):
				subprocess.run(["tar", "-xf", FILE])
			else:
				print(f" File {FILE} not found!")
			print(" Done!")
			print("---------------------------------------------------------------")

		os.chdir("..")

	if app_id == "fraud_detection" or app_id == "all":
		this_app = "fraud_detection"
		if not os.path.exists("fraud_detection"):
			os.mkdir("fraud_detection")
		os.chdir("fraud_detection")

		print(" Building Fraud Detection input classes...")

		FILE = "fraud_detection.tar.gz"
		if force and os.path.isfile(FILE):
			os.remove(FILE)
		if not os.path.isfile(FILE):
			print(f" Downloading {FILE}...")
			subprocess.run(["wget", f"{PUB_DATA_URL}/{this_app}/{FILE}"])
		if os.path.isfile(FILE):
			subprocess.run(["tar", "-xf", FILE])
		else:
			print(f" File {FILE} not found!")
		print(" Done!")
		print("---------------------------------------------------------------")

		os.chdir("..")

	if app_id == "person_recognition" or app_id == "all":
		this_app = "person_recognition"
		if not os.path.exists("person_recognition"):
			os.mkdir("person_recognition")
		os.chdir("person_recognition")

		print(" Building Person Recognition input classes...")

		FILE = "person_inputs.tar.gz"
		if force and os.path.isfile(FILE):
			os.remove(FILE)
		if not os.path.isfile(FILE):
			print(f" Downloading {FILE}...")
			subprocess.run(["wget", f"{PUB_DATA_URL}/{this_app}/{FILE}"])
		if os.path.isfile(FILE):
			subprocess.run(["tar", "-xf", FILE])
		else:
			print(f" File {FILE} not found!")
		print(" Done!")
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
			if not os.path.isfile(FILE):
				print(f" Downloading {FILE}...")
				subprocess.run(["wget", f"{PUB_DATA_URL}/{this_app}/{FILE}"])
			print(f" Building {this_app} {this_class} class...")
			if os.path.isfile(FILE):
				subprocess.run(["tar", "-xf", FILE])
			else:
				print(f" File {FILE} not found!")
			print(" Done!")
			print("---------------------------------------------------------------")

		if class_id == "medium" or class_id == "all":
			this_class = "medium"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			if not os.path.isfile(FILE):
				print(f" Downloading {FILE}...")
				subprocess.run(["wget", f"{PUB_DATA_URL}/{this_app}/{FILE}"])
			print(f" Building {this_app} {this_class} class...")
			if os.path.isfile(FILE):
				subprocess.run(["tar", "-xf", FILE])
			else:
				print(f" File {FILE} not found!")
			print(" Done!")
			print("---------------------------------------------------------------")

		if class_id == "large" or class_id == "all":
			this_class = "large"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			if not os.path.isfile(FILE):
				print(f" Downloading {FILE}...")
				subprocess.run(["wget", f"{PUB_DATA_URL}/{this_app}/{FILE}"])
			print(f" Building {this_app} {this_class} class...")
			if os.path.isfile(FILE):
				subprocess.run(["tar", "-xf", FILE])
			else:
				print(f" File {FILE} not found!")
			print(" Done!")
			print("---------------------------------------------------------------")

		if class_id == "huge" or class_id == "all":
			this_class = "huge"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			if not os.path.isfile(FILE):
				print(f" Downloading {FILE}...")
				subprocess.run(["wget", f"{PUB_DATA_URL}/{this_app}/{FILE}"])
			print(f" Building {this_app} {this_class} class...")
			if os.path.isfile(FILE):
				subprocess.run(["tar", "-xf", FILE])
			else:
				print(f" File {FILE} not found!")
			print(" Done!")
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
			if not os.path.isfile(FILE):
				print(f" Downloading {FILE}...")
				subprocess.run(["wget", f"{PUB_DATA_URL}/{this_app}/{FILE}"])
			print(f" Building {this_app} {this_class} class...")
			if os.path.isfile(FILE):
				subprocess.run(["tar", "-xf", FILE])
			else:
				print(f" File {FILE} not found!")
			print(" Done!")
			print("---------------------------------------------------------------")

		if class_id == "small" or class_id == "all":
			this_class = "small"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			if not os.path.isfile(FILE):
				print(f" Downloading {FILE}...")
				subprocess.run(["wget", f"{PUB_DATA_URL}/{this_app}/{FILE}"])
			print(f" Building {this_app} {this_class} class...")
			if os.path.isfile(FILE):
				subprocess.run(["tar", "-xf", FILE])
			else:
				print(f" File {FILE} not found!")
			print(" Done!")
			print("---------------------------------------------------------------")

		if class_id == "medium" or class_id == "all":
			this_class = "medium"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			if not os.path.isfile(FILE):
				print(f" Downloading {FILE}...")
				subprocess.run(["wget", f"{PUB_DATA_URL}/{this_app}/{FILE}"])
			print(f" Building {this_app} {this_class} class...")
			if os.path.isfile(FILE):
				subprocess.run(["tar", "-xf", FILE])
			else:
				print(f" File {FILE} not found!")
			print(" Done!")

		if class_id == "large" or class_id == "all":
			this_class = "large"
			FILE = f"{this_class}.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			if not os.path.isfile(FILE):
				print(f" Downloading {FILE}...")
				subprocess.run(["wget", f"{PUB_DATA_URL}/{this_app}/{FILE}"])
			print(f" Building {this_app} {this_class} class...")
			if os.path.isfile(FILE):
				subprocess.run(["tar", "-xf", FILE])
			else:
				print(f" File {FILE} not found!")
			print(" Done!")
			print("---------------------------------------------------------------")

		if class_id == "huge" or class_id == "all":
			this_class = "huge"
			FILE = "large.tar.gz"
			if force and os.path.isfile(FILE):
				os.remove(FILE)
			if not os.path.isfile(FILE):
				print(f" Downloading {FILE}...")
				subprocess.run(["wget", f"{PUB_DATA_URL}/{this_app}/{FILE}"])
			print(f" Building {this_app} {this_class} class...")
			if os.path.isfile(FILE):
				subprocess.run(["tar", "-xf", FILE])
			else:
				print(f" File {FILE} not found!")
			subprocess.run(["cp", "enwiki-20211120-pages-articles-multistream9.xml", "enwiki-20211120-pages-articles-multistream9-2x.xml"])
			subprocess.run(["cat", "enwiki-20211120-pages-articles-multistream9.xml", ">>", "enwiki-20211120-pages-articles-multistream9-2x.xml"])
			print(" Done!")
			print("---------------------------------------------------------------")

		os.chdir(spbench_path)