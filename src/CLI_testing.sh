# This script runs some basic operations in SPbench
# for looking for errors 

cd ..

APP_NAME=test_app
./spbench -h
./spbench list
./spbench list-inputs
./spbench list-op -bench lane_sequential
./spbench new-app -app $APP_NAME -operators operator1 operator2 operator3
./spbench new -bench test -ppi test -app $APP_NAME
./spbench new -bench test2 -ppi test -app $APP_NAME -from test
./spbench list-op -bench test2
./spbench delete -bench test
./spbench rename -old-name test2 -new-name test
./spbench clean -bench test
./spbench update -bench test
./spbench compile -bench test
./spbench edit -bench test -editor cat
./spbench configure -bench test -editor cat
./spbench global-config -editor cat
./spbench update -bench test
./spbench compile -bench test

# generate a file with a list of 20 random numbers
FILE=inputs/$APP_NAME/numbers.txt
mkdir -p inputs/$APP_NAME
for i in {1..20}; do echo $RANDOM >> $FILE; done
./spbench new-input -id small -app $APP_NAME -input-string "\$SPB_HOME/$FILE 20" -md5 "myasdasdtestjfaksdmd5"

./spbench exec -bench test -nth 4 -input small -thr -freq-patt spike,10,5,10,100 -batch 2 -monitor 250 -repeat 2 -quiet
./spbench exec -bench test -nth 4 -input small -latency -freq-patt wave,2,10,50 -batch 2 -batch-interval 0.2 -monitor 250 -repeat 2 -test
./spbench exec -bench test -nth 3:4 -input small -thr -freq-patt binary,1,20,100 -monitor 500 -test -in-mem
./spbench exec -bench test -nth 2:4 -input small -latency -thr -freq-patt increasing,10,10,20 -batch-interval 0.3 -repeat 3 -user thr
./spbench clean -bench test
./spbench delete -bench test
./spbench delete-input -id small -app $APP_NAME

cat log/test_2-4.dat
cd sys
