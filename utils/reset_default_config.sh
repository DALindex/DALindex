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
echo "#define FANOUT_BP 64" >> src/config.hpp;
echo "#define AUTO_TUNE_RATE 0.1" >> src/config.hpp;
echo "#define AUTO_TUNE_SIZE AUTO_TUNE_RATE * TIME_WINDOW" >> src/config.hpp;
echo "#define MIN_BOUND" >> src/config.hpp;
echo "#define TUNE" >> src/config.hpp;