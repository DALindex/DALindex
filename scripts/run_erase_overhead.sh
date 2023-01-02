# Place Holder data directory absolute path
DIR=/

for FILE_NAME_R in f_books f_fb f_lognormal f_normal f_udense f_usparse f_wiki f_osm; do
    echo "ALEX: $FILE_NAME_R";
    bash utils/reset_default_config.sh;
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
    g++ benchmark/run_erase_overhead.cpp -std=c++17 -march=native -fopenmp -O3 -w -o z_run_test && ./z_run_test >> benchmark/exp_erase_overhead.txt
done

for FILE_NAME_R in f_books f_fb f_lognormal f_normal f_udense f_usparse f_wiki f_osm; do
    for FANOUT in 16 64 256 1024; do
        echo "B+Tree: $FILE_NAME_R, $FANOUT";
        > src/config.hpp; #Default parameters, just resetting
        echo "#pragma once" >> src/config.hpp;
        echo "#define EXPANSION_RATIO 1.05" >> src/config.hpp;
        echo "#define MAX_BUFFER_SIZE 256" >> src/config.hpp;
        echo "#define MAX_NO_SHIFT MAX_BUFFER_SIZE" >> src/config.hpp;
        echo "#define MIN_OCCUPANCY 0.5" >> src/config.hpp;
        echo "#define MAX_OCCUPANCY 0.8" >> src/config.hpp;
        echo "#define GLOBAL_ERROR 256" >> src/config.hpp;
        echo "#define NO_SEGMENT 128" >> src/config.hpp;
        echo "#define SEARCH_BOUND_RATIO 0.6" >> src/config.hpp;
        echo "#define FANOUT_BP $FANOUT" >> src/config.hpp;
        echo "#define AUTO_TUNE_RATE 0.1" >> src/config.hpp;
        echo "#define AUTO_TUNE_SIZE AUTO_TUNE_RATE * TIME_WINDOW" >> src/config.hpp;
        echo "#define MIN_BOUND" >> src/config.hpp;
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
        echo "#define RUNFLAG 1" >> parameters.hpp;
        g++ benchmark/run_erase_overhead.cpp -std=c++17 -march=native -fopenmp -O3 -w -o z_run_test && ./z_run_test >> benchmark/exp_erase_overhead.txt
    done
done

for FILE_NAME_R in f_books f_fb f_lognormal f_normal f_udense f_usparse f_wiki f_osm; do
    for FANOUT in 16 64 256 1024; do
        echo "PGM: $FILE_NAME_R, $FANOUT";
        > src/config.hpp; #Default parameters, just resetting
        echo "#pragma once" >> src/config.hpp;
        echo "#define EXPANSION_RATIO 1.05" >> src/config.hpp;
        echo "#define MAX_BUFFER_SIZE 256" >> src/config.hpp;
        echo "#define MAX_NO_SHIFT MAX_BUFFER_SIZE" >> src/config.hpp;
        echo "#define MIN_OCCUPANCY 0.5" >> src/config.hpp;
        echo "#define MAX_OCCUPANCY 0.8" >> src/config.hpp;
        echo "#define GLOBAL_ERROR 256" >> src/config.hpp;
        echo "#define NO_SEGMENT 128" >> src/config.hpp;
        echo "#define SEARCH_BOUND_RATIO 0.6" >> src/config.hpp;
        echo "#define FANOUT_BP $FANOUT" >> src/config.hpp;
        echo "#define AUTO_TUNE_RATE 0.1" >> src/config.hpp;
        echo "#define AUTO_TUNE_SIZE AUTO_TUNE_RATE * TIME_WINDOW" >> src/config.hpp;
        echo "#define MIN_BOUND" >> src/config.hpp;
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
        echo "#define RUNFLAG 2" >> parameters.hpp;
        g++ benchmark/run_erase_overhead.cpp -std=c++17 -march=native -fopenmp -O3 -w -o z_run_test && ./z_run_test >> benchmark/exp_erase_overhead.txt
    done
done