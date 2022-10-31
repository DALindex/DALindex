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
#include "../src/ALEX.hpp"
#include "../src/BTree.hpp"
#include "../src/PGM.hpp"
#include "../src/CARMI.hpp"
#include "../src/IMTree.hpp"
#include "../utils/load_with_timestamp.cpp"
#include "../timer/rdtsc.h"
#include "../utils/PerfEvent.hpp"

using namespace std;

// #define RW_RATIO 10

//RW_RATIO multiplied by 10 as #if does not support floating number comparation.
#if RW_RATIO == 1
int noUpdates = 20;
int noSearch = 4;
int noExecution = 1540000;
#elif RW_RATIO == 10
int noUpdates = 11;
int noSearch = 22;
int noExecution = 2800000;
#elif RW_RATIO == 100
int noUpdates = 2;
int noSearch = 40;
int noExecution = 15400000;
#endif

void load_data(vector<tuple<uint64_t, uint64_t, uint64_t>> & data)
{
    string input_file = DATA_DIR FILE_NAME;
    add_timestamp_squential(input_file, data, MATCH_RATE ,SEED);
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

    int updateCount = 0;

    #ifdef TUNE
    DALtuner splitErrorTuner(GLOBAL_ERROR);
    int tuneCounter = 0;
    #endif

    uint64_t maxTimestamp =  get<1>(data.back());
    srand(1); //When using searchTuple

    int pos = TIME_WINDOW;
    int cnt = 0;
    while (cnt != noExecution)
    {
        cnt++;
        for (int ii = 0; ii < noUpdates; ii++)
        {
            pair<uint64_t,uint64_t> insertTuple = make_pair(get<0>(*it),get<1>(*it));
            
            uint64_t tempInsertCycles = 0;
            startTimer(&tempInsertCycles);
            DALindex.tree_insert(insertTuple);
            stopTimer(&tempInsertCycles);
            insertCycle += tempInsertCycles;

            it++;
            pos++;
            itDelete++;

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
        
        
        int minPos = pos -TIME_WINDOW;
        int maxPos = pos;
        
        for (int ii = 0; ii < noSearch; ii++)
        {
            tuple<uint64_t,uint64_t,uint64_t> searchTuple = data.at(minPos + (rand() % (maxPos - minPos +1)));
            // cout << "**NEW START**:" << get<0>(*it) << "," << get<0>(searchTuple) << "," << i << endl;

            uint64_t tempSearchCycles = 0;
            vector<pair<uint64_t, uint64_t>> searchResult;
            startTimer(&tempSearchCycles);
            DALindex.tree_range_search(searchTuple,searchResult);
            stopTimer(&tempSearchCycles);
            searchCycle += tempSearchCycles;
        }

    }

    totalCycle = searchCycle + insertCycle;

    #ifdef TUNE
    cout << "Algorithm=DALindexTune";
    #else
    cout << "Algorithm=DALindex";
    #endif
    cout << ";Data=" << FILE_NAME << ";MatchRate=" << MATCH_RATE << ";RWRatio=" << (double)RW_RATIO/10;
    cout << ";Fanout=" << 0 << ";SplitError=" << GLOBAL_ERROR << ";TimeWindow=" << TIME_WINDOW << ";NoExecution=" << noExecution;
    cout << ";SearchTime=" << (double)searchCycle/CPU_CLOCK;
    cout << ";InsertTime=" << (double)insertCycle/CPU_CLOCK;
    cout << ";DeleteTime=" << 0;
    cout << ";TotalTime=" << (double)totalCycle/CPU_CLOCK << ";";
    cout << endl;
}


void run_alex(vector<tuple<uint64_t, uint64_t, uint64_t>> & data)
{
    vector<pair<uint64_t, uint64_t>> data_initial;
    data_initial.reserve(TIME_WINDOW);
    for (auto it = data.begin(); it != data.begin()+TIME_WINDOW; it++)
    {
        data_initial.push_back(make_pair(get<0>(*it),get<1>(*it)));
    }
    
    vector<pair<uint64_t, uint64_t>> data_initial_sorted = data_initial;
    sort(data_initial_sorted.begin(),data_initial_sorted.end());

    alex::Alex<uint64_t,uint64_t> alex;

    //No Bulk Load 
    for (auto it = data_initial_sorted.begin(); it != data_initial_sorted.end(); it++)
    {
        // alex.insert(*it);
        alex.insert(it->first, it->second);
    }

    auto it = data.begin()+TIME_WINDOW;
    auto itDelete = data.begin();

    uint64_t startTime = get<1>(*(data.begin()+TIME_WINDOW));

    if(startTime < 1)
    {
        cout << "ERROR : timestamp is less than 1 " << endl;
    }
    
    uint64_t searchCycle = 0;
    uint64_t insertCycle = 0;
    uint64_t deleteCycle = 0;
    uint64_t totalCycle = 0;

    int updateCount = 0;

    uint64_t maxTimestamp =  get<1>(data.back());
    srand(1); //When using searchTuple
    
    int pos = TIME_WINDOW;
    int cnt = 0;
    while (cnt != noExecution)
    {
        cnt++;
        for (int ii = 0; ii < noUpdates; ii++)
        {            
            pair<uint64_t,uint64_t> insertTuple = make_pair(get<0>(*it),get<1>(*it));
            uint64_t tempInsertCycles = 0;
            startTimer(&tempInsertCycles);
            alex.insert(insertTuple);
            stopTimer(&tempInsertCycles);
            insertCycle += tempInsertCycles;

            it++;
            pos++;

            uint64_t tempDeleteCycles = 0;
            startTimer(&tempDeleteCycles);
            alex.erase(get<0>(*itDelete));
            stopTimer(&tempDeleteCycles);
            deleteCycle += tempDeleteCycles;

            itDelete++;
        }
        
        int minPos = pos -TIME_WINDOW;
        int maxPos = pos;
        
        for (int ii = 0; ii < noSearch; ii++)
        {
            tuple<uint64_t,uint64_t,uint64_t> searchTuple = data.at(minPos + (rand() % (maxPos - minPos +1)));
            // cout << "**NEW START**:" << get<0>(*it) << "," << get<0>(searchTuple) << "," << i << endl;

            uint64_t tempSearchCycles = 0;
            vector<pair<uint64_t, uint64_t>> searchResult;
            startTimer(&tempSearchCycles);
            alex_range_search(alex,searchTuple,searchResult);
            stopTimer(&tempSearchCycles);
            searchCycle += tempSearchCycles;
        }

    }

    totalCycle = searchCycle + insertCycle + deleteCycle;

    cout << "Algorithm=Alex" << ";Data=" << FILE_NAME << ";MatchRate=" << MATCH_RATE << ";RWRatio=" << (double)RW_RATIO/10;
    cout << ";Fanout=" << 0 << ";SplitError=" << 0 << ";TimeWindow=" << TIME_WINDOW << ";NoExecution=" << noExecution;
    cout << ";SearchTime=" << (double)searchCycle/CPU_CLOCK;
    cout << ";InsertTime=" << (double)insertCycle/CPU_CLOCK;
    cout << ";DeleteTime=" << (double)deleteCycle/CPU_CLOCK;
    cout << ";TotalTime=" << (double)totalCycle/CPU_CLOCK << ";";
    cout << endl;
}


void run_pgm(vector<tuple<uint64_t, uint64_t, uint64_t>> & data)
{
    vector<pair<uint64_t, uint64_t>> data_initial;
    data_initial.reserve(TIME_WINDOW);
    for (auto it = data.begin(); it != data.begin()+TIME_WINDOW; it++)
    {
        data_initial.push_back(make_pair(get<0>(*it),get<1>(*it)));
    }
    
    vector<pair<uint64_t, uint64_t>> data_initial_sorted = data_initial;
    sort(data_initial_sorted.begin(),data_initial_sorted.end());

    pgm::DynamicPGMIndex<uint64_t,uint64_t,pgm::PGMIndex<uint64_t,FANOUT_BP>> pgm(data_initial_sorted.begin(),data_initial_sorted.end());

    auto it = data.begin()+TIME_WINDOW;
    auto itDelete = data.begin();

    uint64_t startTime = get<1>(*(data.begin()+TIME_WINDOW));

    if(startTime < 1)
    {
        cout << "ERROR : timestamp is less than 1 " << endl;
    }
    
    uint64_t searchCycle = 0;
    uint64_t insertCycle = 0;
    uint64_t deleteCycle = 0;
    uint64_t totalCycle = 0;

    int updateCount = 0;

    uint64_t maxTimestamp =  get<1>(data.back());
    srand(1); //When using searchTuple
    
    int pos = TIME_WINDOW;
    int cnt = 0;
    while (cnt != noExecution)
    {
        cnt++;
        for (int ii = 0; ii < noUpdates; ii++)
        {            
            uint64_t tempInsertCycles = 0;
            startTimer(&tempInsertCycles);
            pgm.insert_or_assign(get<0>(*it),get<1>(*it));
            stopTimer(&tempInsertCycles);
            insertCycle += tempInsertCycles;

            it++;
            pos++;

            uint64_t tempDeleteCycles = 0;
            startTimer(&tempDeleteCycles);
            pgm.erase(get<0>(*itDelete));
            stopTimer(&tempDeleteCycles);
            deleteCycle += tempDeleteCycles;

            itDelete++;
        }
        
        int minPos = pos -TIME_WINDOW;
        int maxPos = pos;
        
        for (int ii = 0; ii < noSearch; ii++)
        {
            tuple<uint64_t,uint64_t,uint64_t> searchTuple = data.at(minPos + (rand() % (maxPos - minPos +1)));
            // cout << "**NEW START**:" << get<0>(*it) << "," << get<0>(searchTuple) << "," << i << endl;

            uint64_t tempSearchCycles = 0;
            vector<pair<uint64_t, uint64_t>> searchResult;
            startTimer(&tempSearchCycles);
            pgm_range_search(pgm,searchTuple,searchResult);
            stopTimer(&tempSearchCycles);
            searchCycle += tempSearchCycles;
        }

    }

    totalCycle = searchCycle + insertCycle + deleteCycle;

    cout << "Algorithm=PGM" << ";Data=" << FILE_NAME << ";MatchRate=" << MATCH_RATE << ";RWRatio=" << (double)RW_RATIO/10;
    cout << ";Fanout=" << FANOUT_BP << ";SplitError=" << 0 << ";TimeWindow=" << TIME_WINDOW << ";NoExecution=" << noExecution;
    cout << ";SearchTime=" << (double)searchCycle/CPU_CLOCK;
    cout << ";InsertTime=" << (double)insertCycle/CPU_CLOCK;
    cout << ";DeleteTime=" << (double)deleteCycle/CPU_CLOCK;
    cout << ";TotalTime=" << (double)totalCycle/CPU_CLOCK << ";";
    cout << endl;
}

void run_btree(vector<tuple<uint64_t, uint64_t, uint64_t>> & data)
{
    vector<pair<uint64_t, uint64_t>> data_initial;
    data_initial.reserve(TIME_WINDOW);
    for (auto it = data.begin(); it != data.begin()+TIME_WINDOW; it++)
    {
        data_initial.push_back(make_pair(get<0>(*it),get<1>(*it)));
    }
    
    vector<pair<uint64_t, uint64_t>> data_initial_sorted = data_initial;
    sort(data_initial_sorted.begin(),data_initial_sorted.end());

    stx::btree_map<uint64_t,uint64_t,less<uint64_t>,btree_traits_fanout<uint64_t>> btree(data_initial_sorted.begin(),data_initial_sorted.end());
    
    auto it = data.begin()+TIME_WINDOW;
    auto itDelete = data.begin();

    uint64_t startTime = get<1>(*(data.begin()+TIME_WINDOW));

    if(startTime < 1)
    {
        cout << "ERROR : timestamp is less than 1 " << endl;
    }
    
    uint64_t searchCycle = 0;
    uint64_t insertCycle = 0;
    uint64_t deleteCycle = 0;
    uint64_t totalCycle = 0;

    int updateCount = 0;

    uint64_t maxTimestamp =  get<1>(data.back());
    srand(1); //When using searchTuple
    
    int pos = TIME_WINDOW;
    int cnt = 0;
    while (cnt != noExecution)
    {
        cnt++;
        for (int ii = 0; ii < noUpdates; ii++)
        {            
            pair<uint64_t,uint64_t> insertTuple = make_pair(get<0>(*it),get<1>(*it));
            uint64_t tempInsertCycles = 0;
            startTimer(&tempInsertCycles);
            btree.insert(insertTuple);
            stopTimer(&tempInsertCycles);
            insertCycle += tempInsertCycles;

            it++;
            pos++;

            uint64_t tempDeleteCycles = 0;
            startTimer(&tempDeleteCycles);
            btree.erase(get<0>(*itDelete));
            stopTimer(&tempDeleteCycles);
            deleteCycle += tempDeleteCycles;

            itDelete++;
        }
        
        int minPos = pos -TIME_WINDOW;
        int maxPos = pos;
        
        for (int ii = 0; ii < noSearch; ii++)
        {
            tuple<uint64_t,uint64_t,uint64_t> searchTuple = data.at(minPos + (rand() % (maxPos - minPos +1)));
            // cout << "**NEW START**:" << get<0>(*it) << "," << get<0>(searchTuple) << "," << i << endl;

            uint64_t tempSearchCycles = 0;
            vector<pair<uint64_t, uint64_t>> searchResult;
            startTimer(&tempSearchCycles);
            bt_range_search(btree,searchTuple,searchResult);
            stopTimer(&tempSearchCycles);
            searchCycle += tempSearchCycles;
        }

    }

    totalCycle = searchCycle + insertCycle + deleteCycle;

    cout << "Algorithm=BTree" << ";Data=" << FILE_NAME << ";MatchRate=" << MATCH_RATE << ";RWRatio=" << (double)RW_RATIO/10;
    cout << ";Fanout=" << FANOUT_BP << ";SplitError=" << 0 << ";TimeWindow=" << TIME_WINDOW << ";NoExecution=" << noExecution;
    cout << ";SearchTime=" << (double)searchCycle/CPU_CLOCK;
    cout << ";InsertTime=" << (double)insertCycle/CPU_CLOCK;
    cout << ";DeleteTime=" << (double)deleteCycle/CPU_CLOCK;
    cout << ";TotalTime=" << (double)totalCycle/CPU_CLOCK << ";";
    cout << endl;
}

void run_carmi(vector<tuple<uint64_t, uint64_t, uint64_t>> & data)
{
    vector<pair<uint64_t, uint64_t>> data_initial;
    data_initial.reserve(TIME_WINDOW);
    for (auto it = data.begin(); it != data.begin()+TIME_WINDOW; it++)
    {
        data_initial.push_back(make_pair(get<0>(*it),get<1>(*it)));
    }
    
    vector<pair<uint64_t, uint64_t>> data_initial_sorted = data_initial;
    sort(data_initial_sorted.begin(),data_initial_sorted.end());

    CARMIMap<uint64_t,uint64_t> carmi(data_initial_sorted.begin(),data_initial_sorted.end());
    
    auto it = data.begin()+TIME_WINDOW;
    auto itDelete = data.begin();

    uint64_t startTime = get<1>(*(data.begin()+TIME_WINDOW));

    if(startTime < 1)
    {
        cout << "ERROR : timestamp is less than 1 " << endl;
    }
    
    uint64_t searchCycle = 0;
    uint64_t insertCycle = 0;
    uint64_t deleteCycle = 0;
    uint64_t totalCycle = 0;

    int updateCount = 0;

    uint64_t maxTimestamp =  get<1>(data.back());
    srand(1); //When using searchTuple
    
    int pos = TIME_WINDOW;
    int cnt = 0;
    while (cnt != noExecution)
    {
        cnt++;
        for (int ii = 0; ii < noUpdates; ii++)
        {            
            pair<uint64_t,uint64_t> insertTuple = make_pair(get<0>(*it),get<1>(*it));
            uint64_t tempInsertCycles = 0;
            startTimer(&tempInsertCycles);
            carmi.insert(insertTuple);
            stopTimer(&tempInsertCycles);
            insertCycle += tempInsertCycles;

            it++;
            pos++;

            uint64_t tempDeleteCycles = 0;
            startTimer(&tempDeleteCycles);
            carmi.erase(get<0>(*itDelete));
            stopTimer(&tempDeleteCycles);
            deleteCycle += tempDeleteCycles;

            itDelete++;
        }
        
        int minPos = pos -TIME_WINDOW;
        int maxPos = pos;
        
        for (int ii = 0; ii < noSearch; ii++)
        {
            tuple<uint64_t,uint64_t,uint64_t> searchTuple = data.at(minPos + (rand() % (maxPos - minPos +1)));
            // cout << "**NEW START**:" << get<0>(*it) << "," << get<0>(searchTuple) << "," << i << endl;

            uint64_t tempSearchCycles = 0;
            vector<pair<uint64_t, uint64_t>> searchResult;
            startTimer(&tempSearchCycles);
            carmi_range_search(carmi,searchTuple,searchResult);
            stopTimer(&tempSearchCycles);
            searchCycle += tempSearchCycles;
        }

    }

    totalCycle = searchCycle + insertCycle + deleteCycle;

    cout << "Algorithm=CARMI" << ";Data=" << FILE_NAME << ";MatchRate=" << MATCH_RATE << ";RWRatio=" << (double)RW_RATIO/10;
    cout << ";Fanout=" << 0 << ";SplitError=" << 0 << ";TimeWindow=" << TIME_WINDOW << ";NoExecution=" << noExecution;
    cout << ";SearchTime=" << (double)searchCycle/CPU_CLOCK;
    cout << ";InsertTime=" << (double)insertCycle/CPU_CLOCK;
    cout << ";DeleteTime=" << (double)deleteCycle/CPU_CLOCK;
    cout << ";TotalTime=" << (double)totalCycle/CPU_CLOCK << ";";
    cout << endl;
}

void run_imtree(vector<tuple<uint64_t, uint64_t, uint64_t>> & data)
{
    vector<pair<uint64_t, uint64_t>> data_initial;
    data_initial.reserve(TIME_WINDOW);
    for (auto it = data.begin(); it != data.begin()+TIME_WINDOW; it++)
    {
        data_initial.push_back(make_pair(get<0>(*it),get<1>(*it)));
    }

    IMTree<uint64_t,uint64_t> imtree(data_initial,static_cast<int>((double)TIME_WINDOW*0.125));
    
    auto it = data.begin()+TIME_WINDOW;
    auto itDelete = data.begin();

    uint64_t startTime = get<1>(*(data.begin()+TIME_WINDOW));

    if(startTime < 1)
    {
        cout << "ERROR : timestamp is less than 1 " << endl;
    }
    
    uint64_t searchCycle = 0;
    uint64_t insertCycle = 0;
    uint64_t deleteCycle = 0;
    uint64_t totalCycle = 0;

    int updateCount = 0;

    uint64_t maxTimestamp =  get<1>(data.back());
    srand(1); //When using searchTuple
    

    int pos = TIME_WINDOW;
    int cnt = 0;
    while (cnt != noExecution)
    {
        cnt++;
        for (int ii = 0; ii < noUpdates; ii++)
        {            
            pair<uint64_t,uint64_t> insertTuple = make_pair(get<0>(*it),get<1>(*it));
            uint64_t tempInsertCycles = 0;
            startTimer(&tempInsertCycles);
            imtree.insert(insertTuple);
            stopTimer(&tempInsertCycles);
            insertCycle += tempInsertCycles;


            it++;
            pos++;
            itDelete++;
        }
        
        int minPos = pos -TIME_WINDOW;
        int maxPos = pos;
        
        for (int ii = 0; ii < noSearch; ii++)
        {
            tuple<uint64_t,uint64_t,uint64_t> searchTuple = data.at(minPos + (rand() % (maxPos - minPos +1)));
            // cout << "**NEW START**:" << get<0>(*it) << "," << get<0>(searchTuple) << "," << i << endl;

            uint64_t tempSearchCycles = 0;
            vector<pair<uint64_t, uint64_t>> searchResult;
            startTimer(&tempSearchCycles);
            imtree.range_search(searchTuple,searchResult);
            stopTimer(&tempSearchCycles);
            searchCycle += tempSearchCycles;
        }

    }

    totalCycle = searchCycle + insertCycle;

    cout << "Algorithm=IMTree" << ";Data=" << FILE_NAME << ";MatchRate=" << MATCH_RATE << ";RWRatio=" << (double)RW_RATIO/10;
    cout << ";Fanout=" << 0 << ";SplitError=" << 0 << ";TimeWindow=" << TIME_WINDOW << ";NoExecution=" << noExecution;
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
    
    #ifdef RUNFLAG
    switch (RUNFLAG)
    {
        case 0:
            run_dalindex(data);
            break;
        case 1:
            run_alex(data);
            break;
        case 2:
            run_pgm(data);
            run_btree(data);
            break;
        case 3:
            // run_carmi(data); //Segmentation fault
            run_imtree(data);
            break;
    }
    #else
    run_dalindex(data);
    run_alex(data);
    run_pgm(data);
    run_btree(data);
    // run_carmi(data);
    run_imtree(data);
    #endif;

    return 0;
}