#!/bin/bash

wget https://gmap.pucrs.br/spbench/libs/SPar/spar.tar.gz

tar -xf spar.tar.gz
make

rm spar.tar.gz