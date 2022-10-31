# Place Holder data directory absolute path
DIR=/

echo "Running DALindex Bulkload (No autotune)"
for FILE_NAME in f_books f_fb f_lognormal f_normal f_udense f_usparse f_wiki f_osm; do
    for BULKLOAD in 0 1 2; do
        for TEST_LEN in 2 4 8; do
            for GLOBAL_ERROR in 128 256 512; do
                echo "DALindex: $FILE_NAME, $GLOBAL_ERROR, $BULKLOAD, $TEST_LEN";
                bash utils/reset_default_config.sh;
                echo "#define BULKLOAD $BULKLOAD" >> src/config.hpp;
                > parameters.hpp;
                echo "#pragma once" >> parameters.hpp;
                echo "#define DATA_DIR \"$DIR\"" >> parameters.hpp;
                echo "#define FILE_NAME \"$FILE_NAME\"" >> parameters.hpp;
                echo "#define TIME_WINDOW 10000000" >> parameters.hpp;
                echo "#define MATCH_RATE 1000" >> parameters.hpp;
                echo "#define TEST_LEN TIME_WINDOW * $TEST_LEN" >> parameters.hpp;
                echo "#define MAX_TIMESTAMP TEST_LEN*2 +1" >> parameters.hpp;
                echo "#define SEED 1" >> parameters.hpp;
                echo "#define CPU_CLOCK 3400000000" >> parameters.hpp;
                echo "#define RUNFLAG 0" >> parameters.hpp;
                echo "#define TIME_BREAKDOWN" >> parameters.hpp;
                g++ benchmark/run_DALindex_bulkload.cpp -std=c++17 -march=native -fopenmp -O3 -w -o z_run_test && ./z_run_test >> benchmark/exp_DALindex_bulkload.txt                        
            done
        done
    done
done
