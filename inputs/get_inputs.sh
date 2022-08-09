# !/bin/bash


THIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
cd $THIS_DIR

APP=''
CLASS=''

if [ -z "$1" ]
then
    APP="all"
else
	APP=$1
fi

if [ -z "$2" ]
then
	CLASS="all"
else
    CLASS=$2
fi

if [[ $APP == "lane_detection" || $APP == "all" ]]; then

	THIS_APP="lane_detection"

	if [ ! -d "lane_detection" ]; then
		mkdir $THIS_APP
	fi

	#cd $THIS_APP

	FILE=lane_inputs.tar.gz

	if [[ $3 == "force" ]]; then
		rm $FILE
	fi

	if [ ! -f "$FILE" ]; then
		echo " Downloading $FILE..."
		wget https://gmap.pucrs.br/public_data/spbench/workloads/lane/$FILE
	fi
	echo " Building Lane Detection input classes..."
	tar -xf $FILE
	#cd ..
fi

if [[ $APP == "person_recognition" || $APP == "all" ]]
then
	THIS_APP="person_recognition"

	if [ ! -d "person_recognition" ]; then
		mkdir $THIS_APP
	fi

	#cd $THIS_APP
	echo " Building Person Recognition input classes..."
	FILE=person_inputs.tar.gz

	if [[ $3 == "force" ]]; then
		rm $FILE
	fi
	
	if [ ! -f "$FILE" ]; then
		echo " Downloading $FILE..."
		wget https://gmap.pucrs.br/public_data/spbench/workloads/person/$FILE
	fi



	tar -xf $FILE
	#cd ..
fi

if [[ $APP == "ferret" || $APP == "all" ]]
then
	THIS_APP="ferret"

	if [ ! -d "ferret" ]; then
		mkdir $THIS_APP
	fi

	#cd $THIS_APP

	FILE=ferret_inputs.tar.gz

	if [[ $3 == "force" ]]; then
		rm $FILE
	fi

	if [ ! -f "$FILE" ]; then
		echo " Downloading $FILE..."
		wget https://gmap.pucrs.br/public_data/spbench/workloads/ferret/$FILE
	fi
	echo " Building Ferret input classes..."
	tar -xf $FILE
	#cd ..
fi

if [[ $APP == "bzip2" || $APP == "all" ]]
then

	THIS_APP="bzip2"

	if [ ! -d "bzip2" ]; then
		mkdir $THIS_APP
	fi

	cd $THIS_APP

	if [[ $CLASS == "test" || $CLASS == "all" ]]
	then
		FILE=bzip2_test.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget https://gmap.pucrs.br/public_data/spbench/workloads/bzip2/$FILE
		fi
		echo " Building bzip2 test class..."
		tar -xf $FILE
	fi

	if [[ $CLASS == "small" || $CLASS == "all" ]]
	then
		FILE=bzip2_small.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget https://gmap.pucrs.br/public_data/spbench/workloads/bzip2/$FILE
		fi
		echo " Building bzip2 small class..."
		tar -xf $FILE
	fi

	if [[ $CLASS == "medium" || $CLASS == "all" ]]
	then
		FILE=bzip2_medium.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget https://gmap.pucrs.br/public_data/spbench/workloads/bzip2/$FILE
		fi
		echo " Building bzip2 medium class..."
		tar -xf $FILE
	fi
	
	if [[ $CLASS == "large" || $CLASS == "all" ]]
	then
		FILE=bzip2_large.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget https://gmap.pucrs.br/public_data/spbench/workloads/bzip2/$FILE
		fi
		echo " Building bzip2 large class..."
		tar -xf $FILE
	fi

	if [[ $CLASS == "huge" || $CLASS == "all" ]]
	then
		FILE=bzip2_large.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget https://gmap.pucrs.br/public_data/spbench/workloads/bzip2/$FILE
		fi
		tar -xf $FILE

		echo " Building bzip2 huge class..."

		cat enwiki-20211120-pages-articles-multistream9.xml > enwiki-20211120-pages-articles-multistream9-2x.xml
		cat enwiki-20211120-pages-articles-multistream9.xml >> enwiki-20211120-pages-articles-multistream9-2x.xml
	fi

	cd ..
fi
