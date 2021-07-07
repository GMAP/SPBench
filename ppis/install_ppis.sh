#!/bin/bash

cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"

echo "---------------------------------------"
echo " Installing FastFlow..."
echo "---------------------------------------"
cd fastflow
bash setup_fastflow.sh
echo "DONE!"

echo "---------------------------------------"
echo " Installing SPAR..."
echo "---------------------------------------"
cd ../SPar
bash setup_spar.sh
make
echo "DONE!"

echo "---------------------------------------"
echo " Installing TBB..."
echo "---------------------------------------"
cd ../tbb
bash setup_tbb.sh
cd ..
echo "DONE!"