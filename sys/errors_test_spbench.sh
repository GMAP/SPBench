# This script runs some basic operations in SPbench
# for looking for errors 

../spbench -h
../spbench list
../spbench list-inputs
../spbench list-op -bench lane_sequential
../spbench new -bench teste -ppi teste -app lane_detection
../spbench new -bench teste2 -ppi teste -app lane_detection -from teste
../spbench list-op -bench teste2
../spbench delete -bench teste
../spbench rename -old-name teste2 -new-name teste
../spbench clean -bench teste
../spbench compile -bench teste
../spbench edit -bench teste -editor cat
../spbench configure -bench teste -editor cat
../spbench global-config -editor cat
../spbench update -bench teste
../spbench compile -bench teste
../spbench new-input -id teste -app lane_detection -input-string "$BENCH_DIR/my_test/string" -md5 "myasdasdtestjfaksdmd5"
../spbench delete-input -id teste -app lane_detection
../spbench exec -bench teste -nth 4 -input test -latency -thr -freq-patt spike,10,5,10,100 -batch 2 -monitor 250 -repeat 2
../spbench clean -bench teste
../spbench delete -bench teste
