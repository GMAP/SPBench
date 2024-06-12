#!/usr/bin/env bash


THIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
cd $THIS_DIR

APP=''
CLASS=''
PUB_DATA_URL="https://gmap.pucrs.br/public_data/spbench/workloads"

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

echo "---------------------------------------------------------------"
if [[ $APP == "lane_detection" || $APP == "all" ]]; then

	THIS_APP="lane_detection"

	if [ ! -d "lane_detection" ]; then
		mkdir $THIS_APP
	fi

	cd $THIS_APP

	if [[ $CLASS == "test" || $CLASS == "all" ]]
	then
		THIS_CLASS=test
		FILE=$THIS_CLASS.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget $PUB_DATA_URL/$THIS_APP/$FILE
		fi
		echo " Building $THIS_APP $THIS_CLASS class..."
		tar -xf $FILE
		echo " Done!"
		echo "---------------------------------------------------------------"
	fi

	if [[ $CLASS == "small" || $CLASS == "all" ]]
	then
		THIS_CLASS=small
		FILE=$THIS_CLASS.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget $PUB_DATA_URL/$THIS_APP/$FILE
		fi
		echo " Building $THIS_APP $THIS_CLASS class..."
		tar -xf $FILE
		echo " Done!"
		echo "---------------------------------------------------------------"
	fi

	if [[ $CLASS == "medium" || $CLASS == "all" ]]
	then
		THIS_CLASS=medium
		FILE=$THIS_CLASS.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget $PUB_DATA_URL/$THIS_APP/$FILE
		fi
		echo " Building $THIS_APP $THIS_CLASS class..."
		tar -xf $FILE
		echo " Done!"
		echo "---------------------------------------------------------------"
	fi
	
	if [[ $CLASS == "large" || $CLASS == "all" ]]
	then
		THIS_CLASS=large
		FILE=$THIS_CLASS.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget $PUB_DATA_URL/$THIS_APP/$FILE
		fi
		echo " Building $THIS_APP $THIS_CLASS class..."
		tar -xf $FILE
		echo " Done!"
		echo "---------------------------------------------------------------"
	fi

	if [[ $CLASS == "huge" || $CLASS == "all" ]]
	then
		THIS_CLASS=huge
		FILE=$THIS_CLASS.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget $PUB_DATA_URL/$THIS_APP/$FILE
		fi
		
		echo " Building $THIS_APP $THIS_CLASS class..."
		tar -xf $FILE	
		echo " Done!"
		echo "---------------------------------------------------------------"
	fi

	cd ..
fi

if [[ $APP == "fraud_detection" || $APP == "all" ]]
then
	THIS_APP="fraud_detection"

	if [ ! -d "fraud_detection" ]; then
		mkdir $THIS_APP
	fi

	cd $THIS_APP
	
	echo " Building Fraud Detection input classes..."
	
	FILE=fraud_detection.tar.gz

	if [[ $3 == "force" ]]; then
		rm $FILE
	fi
	
	if [ ! -f "$FILE" ]; then
		echo " Downloading $FILE..."
		wget $PUB_DATA_URL/$THIS_APP/$FILE
	fi

	tar -xf $FILE
	echo " Done!"
	echo "---------------------------------------------------------------"

	cd ..
fi

if [[ $APP == "person_recognition" || $APP == "all" ]]
then
	THIS_APP="person_recognition"

	if [ ! -d "person_recognition" ]; then
		mkdir $THIS_APP
	fi

	cd $THIS_APP
	
	echo " Building Person Recognition input classes..."
	
	FILE=person_inputs.tar.gz

	if [[ $3 == "force" ]]; then
		rm $FILE
	fi
	
	if [ ! -f "$FILE" ]; then
		echo " Downloading $FILE..."
		wget $PUB_DATA_URL/$THIS_APP/$FILE
	fi

	tar -xf $FILE
	echo " Done!"
	echo "---------------------------------------------------------------"

	cd ..
fi

if [[ $APP == "ferret" || $APP == "all" ]]
then

	THIS_APP="ferret"

	if [ ! -d "ferret" ]; then
		mkdir $THIS_APP
	fi

	cd $THIS_APP

	if [[ $CLASS == "test" || $CLASS == "small" || $CLASS == "all" ]]
	then
		THIS_CLASS=small
		FILE=$THIS_CLASS.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget $PUB_DATA_URL/$THIS_APP/$FILE
		fi
		echo " Building $THIS_APP $THIS_CLASS class..."
		tar -xf $FILE
		echo " Done!"
		echo "---------------------------------------------------------------"
	fi

	if [[ $CLASS == "medium" || $CLASS == "all" ]]
	then
		THIS_CLASS=medium
		FILE=$THIS_CLASS.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget $PUB_DATA_URL/$THIS_APP/$FILE
		fi
		echo " Building $THIS_APP $THIS_CLASS class..."
		tar -xf $FILE
		echo " Done!"
		echo "---------------------------------------------------------------"
	fi
	
	if [[ $CLASS == "large" || $CLASS == "all" ]]
	then
		THIS_CLASS=large
		FILE=$THIS_CLASS.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget $PUB_DATA_URL/$THIS_APP/$FILE
		fi
		echo " Building $THIS_APP $THIS_CLASS class..."
		tar -xf $FILE
		echo " Done!"
		echo "---------------------------------------------------------------"
	fi

	if [[ $CLASS == "huge" || $CLASS == "all" ]]
	then
		THIS_CLASS=huge
		FILE=$THIS_CLASS.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget $PUB_DATA_URL/$THIS_APP/$FILE
		fi
		
		echo " Building $THIS_APP $THIS_CLASS class..."
		tar -xf $FILE	
		echo " Done!"
		echo "---------------------------------------------------------------"
	fi

	cd ..
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
		THIS_CLASS=test
		FILE=$THIS_CLASS.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget $PUB_DATA_URL/$THIS_APP/$FILE
		fi
		echo " Building $THIS_APP $THIS_CLASS class..."
		tar -xf $FILE
		echo " Done!"
		echo "---------------------------------------------------------------"
	fi

	if [[ $CLASS == "small" || $CLASS == "all" ]]
	then
		THIS_CLASS=small
		FILE=$THIS_CLASS.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget $PUB_DATA_URL/$THIS_APP/$FILE
		fi
		echo " Building $THIS_APP $THIS_CLASS class..."
		tar -xf $FILE
		echo " Done!"
		echo "---------------------------------------------------------------"
	fi

	if [[ $CLASS == "medium" || $CLASS == "all" ]]
	then
		THIS_CLASS=medium
		FILE=$THIS_CLASS.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget $PUB_DATA_URL/$THIS_APP/$FILE
		fi
		echo " Building $THIS_APP $THIS_CLASS class..."
		tar -xf $FILE
		echo " Done!"
	fi
	
	if [[ $CLASS == "large" || $CLASS == "all" ]]
	then
		THIS_CLASS=large
		FILE=$THIS_CLASS.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget $PUB_DATA_URL/$THIS_APP/$FILE
		fi
		echo " Building $THIS_APP $THIS_CLASS class..."
		tar -xf $FILE
		echo " Done!"
		echo "---------------------------------------------------------------"
	fi

	if [[ $CLASS == "huge" || $CLASS == "all" ]]
	then
		THIS_CLASS=huge
		FILE=large.tar.gz
		if [[ $3 == "force" ]]; then
			rm $FILE
		fi
		if [ ! -f "$FILE" ]; then
			echo " Downloading $FILE..."
			wget $PUB_DATA_URL/$THIS_APP/$FILE
		fi
		
		echo " Building $THIS_APP $THIS_CLASS class..."
		tar -xf $FILE
		cp enwiki-20211120-pages-articles-multistream9.xml enwiki-20211120-pages-articles-multistream9-2x.xml
		cat enwiki-20211120-pages-articles-multistream9.xml >> enwiki-20211120-pages-articles-multistream9-2x.xml		
		echo " Done!"
		echo "---------------------------------------------------------------"
	fi

	cd ..
fi
