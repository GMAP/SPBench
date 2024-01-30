# This script runs some basic operations in SPbench
# for looking for errors 

cd ..

APP_NAME=test
./spbench -h


printf "@@@@@@@@@@ ./spbench list\n"
./spbench list

printf "@@@@@@@@@@ ./spbench list-inputs\n"
./spbench list-inputs

printf "@@@@@@@@@@ ./spbench list-op -bench lane_sequential\n"
./spbench list-op -bench lane_sequential

printf "@@@@@@@@@@ ./spbench new-app -app $APP_NAME -operators operator1 operator2 operator3\n"
./spbench new-app -app $APP_NAME -operators operator1 operator2 operator3

printf "@@@@@@@@@@ ./spbench new -bench test -ppi test -app $APP_NAME\n"
./spbench new -bench test -ppi test -app $APP_NAME

printf "@@@@@@@@@@ ./spbench new -bench test2 -ppi test -app $APP_NAME -from test\n"
./spbench new -bench test2 -ppi test -app $APP_NAME -from test

printf "@@@@@@@@@@ ./spbench list-op -bench test2\n"
./spbench list-op -bench test2

printf "@@@@@@@@@@ ./spbench delete -bench test\n"
./spbench delete -bench test

printf "@@@@@@@@@@ ./spbench rename -old-name test2 -new-name test\n"
./spbench rename -old-name test2 -new-name test

printf "@@@@@@@@@@ ./spbench clean -bench test\n"
./spbench clean -bench test

printf "@@@@@@@@@@ ./spbench update -bench test\n"
./spbench update -bench test

printf "@@@@@@@@@@ ./spbench compile -bench test\n"
./spbench compile -bench test

printf "@@@@@@@@@@ ./spbench edit -bench test -editor cat\n"
./spbench edit -bench test -editor cat

printf "@@@@@@@@@@ ./spbench configure -bench test -editor cat\n"
./spbench configure -bench test -editor cat

printf "@@@@@@@@@@ ./spbench global-config -editor cat\n"
./spbench global-config -editor cat

printf "@@@@@@@@@@ ./spbench update -bench test\n"
./spbench update -bench test

printf "@@@@@@@@@@ ./spbench compile -bench test\n"
./spbench compile -bench test

printf "@@@@@@@@@@ ./spbench new-input -id small -app $APP_NAME -input-string \"Example input string\" -md5 \"myasdasdtestjfaksdmd5\"\n"
./spbench new-input -id small -app $APP_NAME -input-string "\$SPB_HOME/$FILE 20" -md5 "myasdasdtestjfaksdmd5"

printf "@@@@@@@@@@ ./spbench exec -bench test -nth 4 -input test -thr -freq-patt spike,3,50,100,50 -batch 2 -monitor 250 -repeat 2 -quiet\n"
./spbench exec -bench test -nth 4 -input test -thr -freq-patt spike,10,50,100,50 -batch 2 -monitor 250 -repeat 2 -quiet

printf "@@@@@@@@@@ ./spbench exec -bench test -nth 4 -input test -latency -freq-patt wave,2,50,100 -batch 2 -batch-interval 0.2 -monitor 250 -repeat 2 -test\n"
./spbench exec -bench test -nth 4 -input test -latency -freq-patt wave,2,50,100 -batch 2 -batch-interval 0.2 -monitor 250 -repeat 2 -test

printf "@@@@@@@@@@ ./spbench exec -bench test -nth 3:4 -input test -thr -freq-patt binary,1,50,100 -monitor 500 -test -in-mem\n"
./spbench exec -bench test -nth 3:4 -input test -thr -freq-patt binary,1,50,100 -monitor 500 -test -in-mem

printf "@@@@@@@@@@ ./spbench exec -bench test -nth 2:4 -input test -latency -thr -freq-patt increasing,10,40,100 -batch-interval 0.3 -repeat 3 -user thr\n"
./spbench exec -bench test -nth 2:4 -input test -latency -thr -freq-patt increasing,10,40,20 -batch-interval 0.3 -repeat 3 -user thr

printf "@@@@@@@@@@ ./spbench clean -bench test\n"
./spbench clean -bench test

printf "@@@@@@@@@@ ./spbench delete -bench test\n"
./spbench delete -bench test

printf "@@@@@@@@@@ ./spbench delete-input -id small -app $APP_NAME\n"
./spbench delete-input -id small -app $APP_NAME

printf "@@@@@@@@@@ ./spbench delete-app -app $APP_NAME\n"
./spbench delete-app -app $APP_NAME

cat log/test_2-4.dat
cd sys
