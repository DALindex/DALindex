# Place Holder data directory absolute path
DIR=/

echo "Running DALindex Local Search"
for FILE_NAME in f_books f_fb f_lognormal f_normal f_udense f_usparse f_wiki f_osm; do
    for LOCAL_SEARCH in 0 1 2; do
        echo "DALindex: $FILE_NAME, $LOCAL_SEARCH";
        bash utils/reset_default_config.sh;
        echo "#define LOCAL_SEARCH $LOCAL_SEARCH" >> src/config.hpp;
        > parameters.hpp;
        echo "#pragma once" >> parameters.hpp;
        echo "#define DATA_DIR \"$DIR\"" >> parameters.hpp;
        echo "#define FILE_NAME \"$FILE_NAME\"" >> parameters.hpp;
        echo "#define TIME_WINDOW 10000000" >> parameters.hpp;
        echo "#define MATCH_RATE 1000" >> parameters.hpp;
        echo "#define TEST_LEN TIME_WINDOW * 4" >> parameters.hpp;
        echo "#define MAX_TIMESTAMP TEST_LEN*2 +1" >> parameters.hpp;
        echo "#define SEED 1" >> parameters.hpp;
        echo "#define CPU_CLOCK 3400000000" >> parameters.hpp;
        echo "#define RUNFLAG 0" >> parameters.hpp;
        echo "#define TIME_BREAKDOWN" >> parameters.hpp;
        g++ benchmark/run_DALindex_local_search.cpp -std=c++17 -march=native -fopenmp -O3 -w -o z_run_test && ./z_run_test >> benchmark/exp_DALindex_local_search.txt                        
    done
done