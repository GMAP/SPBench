# !/bin/bash

#ferret all
wget https://gmap.pucrs.br/public_data/spbench/workloads/ferret/ferret_inputs.tar.gz
tar -xf ferret_inputs.tar.gz
rm -f ferret_inputs.tar.gz

#lane all
wget https://gmap.pucrs.br/public_data/spbench/workloads/lane/lane_inputs.tar.gz
tar -xf lane_inputs.tar.gz
rm -f lane_inputs.tar.gz

#person all
wget https://gmap.pucrs.br/public_data/spbench/workloads/person/person_inputs.tar.gz
tar -xf person_inputs.tar.gz
rm -f person_inputs.tar.gz

#Bzip temporary small
mkdir bzip2
cp lane_detection/large.mp4 bzip2/small.mp4

#Bzip2 ISO file
wget https://gmap.pucrs.br/public_data/spbench/workloads/bzip2/bzip2_inputs.tar.gz
tar -xf bzip2_inputs.tar.gz
mkdir bzip2
mv FC-6-x86_64-disc1.iso bzip2/
rm bzip2_inputs.tar.gz
