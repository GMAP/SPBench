#!/bin/bash

wget -c --read-timeout=5 --tries=10 https://gmap.pucrs.br/public_data/spbench/libs/fastflow/fastflow.tar.gz

tar -xf fastflow.tar.gz

rm fastflow.tar.gz
