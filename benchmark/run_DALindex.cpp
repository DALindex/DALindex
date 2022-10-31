#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <random>
#include <cmath>
#include <fstream>

#include "../utils/output_files.hpp"
#include "../src/DALroot.hpp"
#include "../src/DALtuner.hpp"
#include "../utils/load_with_timestamp.cpp"
#include "../timer/rdtsc.h"

using namespace std;

void load_data(vector<tuple<uint64_t, uint64_t, uint64_t>> & data)
{
    string input_file = DATA_DIR FILE_NAME;

    add_timestamp(input_file, data, MATCH_RATE ,SEED);
}


void run_dalindex(vector<tuple<uint64_t, uint64_t, uint64_t>> & data)
{
    vector<pair<uint64_t, uint64_t>> data_initial;
    data_initial.reserve(TIME_WINDOW);
    for (auto it = data.begin(); it != data.begin()+TIME_WINDOW; it++)
    {
        data_initial.push_back(make_pair(get<0>(*it),get<1>(*it)));
    }
    
    DALroot<uint64_t,uint64_t> DALindex(data_initial);

    auto it = data.begin()+TIME_WINDOW;
    auto itDelete = data.begin();

    uint64_t startTime = get<1>(*(data.begin()+TIME_WINDOW));

    if(startTime < 1)
    {
        cout << "ERROR : timestamp is less than 1 " << endl;
    }
    
    uint64_t searchCycle = 0;
    uint64_t insertCycle = 0;
    uint64_t totalCycle = 0;

    #ifdef TUNE
    DALtuner splitErrorTuner(GLOBAL_ERROR);
    int tuneCounter = 0;
    #endif

    uint64_t maxTimestamp =  get<1>(data.back());
    srand(1); //When using searchTuple

    for(uint64_t i = startTime; i < maxTimestamp; i++ )
    {
        while (get<1>(*itDelete) < i-TIME_WINDOW)
        {
            itDelete++;
        }
        
        while(get<1>(*it) == i)
        {
			tuple<uint64_t,uint64_t,uint64_t> searchTuple = data.at((itDelete - data.begin()) + (rand() % ( (it - data.begin()) - (itDelete - data.begin()) + 1 )));
            pair<uint64_t,uint64_t> insertTuple = make_pair(get<0>(*it),get<1>(*it));

            #if MATCH_RATE == 1
            pair<uint64_t,uint64_t> searchPair = make_pair(get<0>(searchTuple), get<1>(searchTuple));
            uint64_t tempSearchCycles = 0;
            startTimer(&tempSearchCycles);
            DALindex.tree_lookup(searchPair);
            stopTimer(&tempSearchCycles);
            searchCycle += tempSearchCycles;
            #else
            uint64_t tempSearchCycles = 0;
            vector<pair<uint64_t, uint64_t>> searchResult;
            startTimer(&tempSearchCycles);
            DALindex.tree_range_search(searchTuple,searchResult);
            stopTimer(&tempSearchCycles);
            searchCycle += tempSearchCycles;
            #endif

            uint64_t tempInsertCycles = 0;
            startTimer(&tempInsertCycles);
            DALindex.tree_insert(insertTuple);
            stopTimer(&tempInsertCycles);
            insertCycle += tempInsertCycles;

            it++;

            #ifdef TUNE
            tuneCounter++;
            if (tuneCounter >= AUTO_TUNE_SIZE && segNoRetrain > 25)
            {
                if (tuneStage == 0)
                {                    
                    // cout << "Initial Tune at : " << loopCounter << endl;
                    DALindex.set_split_error(splitErrorTuner.initial_tune());

                }
                else
                {
                    // cout << "Auto Tune at : " << loopCounter << endl;
                    DALindex.set_split_error(splitErrorTuner.tune());
                }
                tuneCounter = 0;
            }
            #endif

        }
    }

    totalCycle = searchCycle + insertCycle;

    #ifdef TUNE
    cout << "Algorithm=DALindexTune";
    #else
    cout << "Algorithm=DALindex";
    #endif
    cout << ";Data=" << FILE_NAME << ";MatchRate=" << MATCH_RATE;
    cout << ";Fanout=" << 0 << ";SplitError=" << GLOBAL_ERROR << ";TimeWindow=" << TIME_WINDOW << ";UpdateLength=" << (TEST_LEN/TIME_WINDOW)-1;
    #ifdef PRINT_TUNE_RATE
    cout << ";TuneRate=" << AUTO_TUNE_RATE*10;
    #endif
    cout << ";SearchTime=" << (double)searchCycle/CPU_CLOCK;
    cout << ";InsertTime=" << (double)insertCycle/CPU_CLOCK;
    cout << ";DeleteTime=" << 0;
    cout << ";TotalTime=" << (double)totalCycle/CPU_CLOCK << ";";
    cout << endl;
}

int main(int argc, char** argv)
{
    vector<tuple<uint64_t, uint64_t, uint64_t>> data;
    data.reserve(TEST_LEN);
    
    load_data(data);
    
    run_dalindex(data);

    return 0;
}