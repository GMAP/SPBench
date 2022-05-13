#!/bin/bash

wget -c --read-timeout=5 --tries=10 https://gmap.pucrs.br/public_data/spbench/libs/spar/spar.tar.gz

tar -xf spar.tar.gz
make

rm spar.tar.gz
