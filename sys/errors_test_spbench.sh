# This script runs some basic operations in SPbench
# for looking for errors 

cd ..

./spbench -h
./spbench list
./spbench list-inputs
./spbench list-op -bench lane_sequential
./spbench new -bench teste -ppi teste -app lane_detection
./spbench new -bench teste2 -ppi teste -app lane_detection -from teste
./spbench list-op -bench teste2
./spbench delete -bench teste
./spbench rename -old-name teste2 -new-name teste
./spbench clean -bench teste
./spbench compile -bench teste
./spbench edit -bench teste -editor cat
./spbench configure -bench teste -editor cat
./spbench global-config -editor cat
./spbench update -bench teste
./spbench compile -bench teste
./spbench new-input -id teste -app lane_detection -input-string "$SPB_HOME/my_test/string" -md5 "myasdasdtestjfaksdmd5"
./spbench delete-input -id teste -app lane_detection
./spbench exec -bench teste -nth 4 -input test -thr -freq-patt spike,10,5,10,100 -batch 2 -monitor 250 -repeat 2 -quiet
./spbench exec -bench teste -nth 4 -input test -latency -freq-patt wave,2,10,50 -batch 2 -batch-interval 0.2 -monitor 250 -repeat 2 -test
./spbench exec -bench teste -nth 3:4 -input test -thr -freq-patt binary,1,20,100 -monitor 500 -test -in-mem
./spbench clean -bench teste
./spbench delete -bench teste
./spbench exec -bench lane_grppi_farm -nth 2:4 -input small -latency -thr -freq-patt increasing,10,10,20 -batch-interval 0.3 -repeat 3 -user thr
cat log/lane_grppi_farm_2-1-4.dat
cd sys
