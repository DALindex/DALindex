# Place Holder data directory absolute path
DIR=/

echo "Running DALindex Error"
for FILE_NAME in f_books f_fb f_lognormal f_normal f_udense f_usparse f_wiki f_osm; do
    echo "DALindex (TUNE): $FILE_NAME";
    bash utils/reset_default_config.sh;
    > parameters.hpp;
    echo "#pragma once" >> parameters.hpp;
    echo "#define DATA_DIR \"$DIR\"" >> parameters.hpp;
    echo "#define FILE_NAME \"$FILE_NAME\"" >> parameters.hpp;
    echo "#define TIME_WINDOW 10000000" >> parameters.hpp;
    echo "#define MATCH_RATE 1000" >> parameters.hpp;
    echo "#define TEST_LEN TIME_WINDOW * 8" >> parameters.hpp;
    echo "#define MAX_TIMESTAMP TEST_LEN*2 +1" >> parameters.hpp;
    echo "#define SEED 1" >> parameters.hpp;
    echo "#define CPU_CLOCK 3400000000" >> parameters.hpp;
    echo "#define RUNFLAG 0" >> parameters.hpp;
    echo "#define TIME_BREAKDOWN" >> parameters.hpp;
    g++ benchmark/run_DALindex_error.cpp -std=c++17 -march=native -fopenmp -O3 -w -o z_run_test && ./z_run_test >> benchmark/exp_DALindex_error.txt                        
done


