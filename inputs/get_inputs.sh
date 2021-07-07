# !/bin/bash 

#ferret all
wget https://gmap.pucrs.br/spbench/inputs/ferret_inputs.tar.gz
tar -xf ferret_inputs.tar.gz
rm -f ferret_inputs.tar.gz

#lane all
wget https://gmap.pucrs.br/spbench/inputs/lane_inputs.tar.gz
tar -xf lane_inputs.tar.gz
rm -f lane_inputs.tar.gz

#Bzip temporary small
mkdir bzip2
cp lane_detection/large.mp4 bzip2/small.mp4

#Bzip2 ISO file
wget https://gmap.pucrs.br/spar/workloads/bzip2_dedup/ISO_FILE.tar.gz
tar -xf ISO_FILE.tar.gz
mkdir bzip2
mv FC-6-x86_64-disc1.iso bzip2/
rm ISO_FILE.tar.gz
