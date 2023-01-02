#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <random>
#include <numeric>
#include <cmath>
#include <fstream>

#include "../utils/output_files.hpp"
#include "../src/ALEX.hpp"
#include "../src/BTree.hpp"
#include "../src/PGM.hpp"
#include "../src/CARMI.hpp"

#include "../utils/load_with_timestamp.cpp"
#include "../timer/rdtsc.h"

using namespace std;

vector<uint64_t> searchCycleAll;
vector<uint64_t> insertCycleAll;
vector<uint64_t> deleteCycleAll;

void load_data(vector<tuple<uint64_t, uint64_t, uint64_t>> & data)
{
    std::string input_file = DATA_DIR FILE_NAME_R;

    add_timestamp(input_file, data, MATCH_RATE ,SEED_R);
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

    uint64_t maxTimestamp =  get<1>(data.back());
    srand(1); //When using searchTuple
    
    for(uint64_t i = startTime; i < maxTimestamp; i++ )
    {
        while(get<1>(*it) == i)
        {
            tuple<uint64_t,uint64_t,uint64_t> searchTuple = data.at((itDelete - data.begin()) + (rand() % ( (it - data.begin()) - (itDelete - data.begin()) + 1 )));
            pair<uint64_t,uint64_t> insertTuple = make_pair(get<0>(*it),get<1>(*it));

            uint64_t tempDeleteCycles = 0;
            startTimer(&tempDeleteCycles);
            alex_erase(alex,insertTuple);
            stopTimer(&tempDeleteCycles);
            deleteCycle += tempDeleteCycles;
            deleteCycleAll.push_back(tempDeleteCycles);

            uint64_t tempSearchCycles = 0;
            vector<pair<uint64_t, uint64_t>> tempJoinResult;
            startTimer(&tempSearchCycles);
            alex_range_search(alex,searchTuple,tempJoinResult);
            stopTimer(&tempSearchCycles);
            searchCycle += tempSearchCycles;
            searchCycleAll.push_back(tempSearchCycles);

            uint64_t tempInsertCycles = 0;
            startTimer(&tempInsertCycles);
            alex.insert(insertTuple);
            stopTimer(&tempInsertCycles);
            insertCycle += tempInsertCycles;
            insertCycleAll.push_back(tempInsertCycles);

            it++;
        }
    }

    totalCycle = searchCycle + insertCycle + deleteCycle;

    sort(searchCycleAll.begin(),searchCycleAll.end());
    sort(insertCycleAll.begin(),insertCycleAll.end());
    sort(deleteCycleAll.begin(),deleteCycleAll.end());

    cout << "Algorithm=Alex" << ";Data=" << FILE_NAME_R << ";MatchRate=" << MATCH_RATE;
    cout << ";Fanout=" << 0 << ";SplitError=" << 0 << ";TimeWindow=" << TIME_WINDOW << ";UpdateLength=" << (TEST_LEN/TIME_WINDOW)-1;
    cout << ";AvgSearchLatency=" << (reduce(searchCycleAll.begin(), searchCycleAll.end(), 0.0) / searchCycleAll.size())/machine_frequencey;
    cout << ";LastSearchLatency=" << (double)searchCycleAll.back()/machine_frequencey;
    cout << ";AvgInsertLatency=" << (reduce(insertCycleAll.begin(), insertCycleAll.end(), 0.0) / insertCycleAll.size())/machine_frequencey;
    cout << ";LastInsertLatency=" << (double)insertCycleAll.back()/machine_frequencey;
    cout << ";AvgDeleteLatency=" << (reduce(deleteCycleAll.begin(), deleteCycleAll.end(), 0.0) / deleteCycleAll.size())/machine_frequencey;
    cout << ";LastDeleteLatency=" << (double)deleteCycleAll.back()/machine_frequencey;
    cout << ";TotalTime=" << (double)totalCycle/machine_frequencey << ";";
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

    uint64_t maxTimestamp =  get<1>(data.back());
    srand(1); //When using searchTuple
    
    for(uint64_t i = startTime; i < maxTimestamp; i++ )
    {
        while(get<1>(*it) == i)
        {
            tuple<uint64_t,uint64_t,uint64_t> searchTuple = data.at((itDelete - data.begin()) + (rand() % ( (it - data.begin()) - (itDelete - data.begin()) + 1 )));
            pair<uint64_t,uint64_t> insertTuple = make_pair(get<0>(*it),get<1>(*it));

            uint64_t tempDeleteCycles = 0;
            startTimer(&tempDeleteCycles);
            pgm_erase(pgm,insertTuple);
            stopTimer(&tempDeleteCycles);
            deleteCycle += tempDeleteCycles;
            deleteCycleAll.push_back(tempDeleteCycles);

            uint64_t tempSearchCycles = 0;
            vector<pair<uint64_t, uint64_t>> tempJoinResult;
            startTimer(&tempSearchCycles);
            pgm_range_search(pgm,searchTuple,tempJoinResult);
            stopTimer(&tempSearchCycles);
            searchCycle += tempSearchCycles;
            searchCycleAll.push_back(tempSearchCycles);

            uint64_t tempInsertCycles = 0;
            startTimer(&tempInsertCycles);
            pgm.insert_or_assign(get<0>(*it),get<1>(*it));
            stopTimer(&tempInsertCycles);
            insertCycle += tempInsertCycles;
            insertCycleAll.push_back(tempInsertCycles);

            it++;
        }
    }

    totalCycle = searchCycle + insertCycle + deleteCycle;

    sort(searchCycleAll.begin(),searchCycleAll.end());
    sort(insertCycleAll.begin(),insertCycleAll.end());
    sort(deleteCycleAll.begin(),deleteCycleAll.end());

    cout << "Algorithm=PGM" << ";Data=" << FILE_NAME_R << ";MatchRate=" << MATCH_RATE;
    cout << ";Fanout=" << FANOUT_BP << ";SplitError=" << 0 << ";TimeWindow=" << TIME_WINDOW << ";UpdateLength=" << (TEST_LEN/TIME_WINDOW)-1;
    cout << ";AvgSearchLatency=" << (reduce(searchCycleAll.begin(), searchCycleAll.end(), 0.0) / searchCycleAll.size())/machine_frequencey;
    cout << ";LastSearchLatency=" << (double)searchCycleAll.back()/machine_frequencey;
    cout << ";AvgInsertLatency=" << (reduce(insertCycleAll.begin(), insertCycleAll.end(), 0.0) / insertCycleAll.size())/machine_frequencey;
    cout << ";LastInsertLatency=" << (double)insertCycleAll.back()/machine_frequencey;
    cout << ";AvgDeleteLatency=" << (reduce(deleteCycleAll.begin(), deleteCycleAll.end(), 0.0) / deleteCycleAll.size())/machine_frequencey;
    cout << ";LastDeleteLatency=" << (double)deleteCycleAll.back()/machine_frequencey;
    cout << ";TotalTime=" << (double)totalCycle/machine_frequencey << ";";
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

    uint64_t maxTimestamp =  get<1>(data.back());
    srand(1); //When using searchTuple
    
    for(uint64_t i = startTime; i < maxTimestamp; i++ )
    {
        while(get<1>(*it) == i)
        {
            tuple<uint64_t,uint64_t,uint64_t> searchTuple = data.at((itDelete - data.begin()) + (rand() % ( (it - data.begin()) - (itDelete - data.begin()) + 1 )));
            pair<uint64_t,uint64_t> insertTuple = make_pair(get<0>(*it),get<1>(*it));

            uint64_t tempDeleteCycles = 0;
            startTimer(&tempDeleteCycles);
            bt_erase(btree,insertTuple);
            stopTimer(&tempDeleteCycles);
            deleteCycle += tempDeleteCycles;
            deleteCycleAll.push_back(tempDeleteCycles);

            uint64_t tempSearchCycles = 0;
            vector<pair<uint64_t, uint64_t>> tempJoinResult;
            startTimer(&tempSearchCycles);
            bt_range_search(btree,searchTuple,tempJoinResult);
            stopTimer(&tempSearchCycles);
            searchCycle += tempSearchCycles;
            searchCycleAll.push_back(tempSearchCycles);

            uint64_t tempInsertCycles = 0;
            startTimer(&tempInsertCycles);
            btree.insert(insertTuple);
            stopTimer(&tempInsertCycles);
            insertCycle += tempInsertCycles;
            insertCycleAll.push_back(tempInsertCycles);

            it++;
        }
    }

    totalCycle = searchCycle + insertCycle + deleteCycle;

    sort(searchCycleAll.begin(),searchCycleAll.end());
    sort(insertCycleAll.begin(),insertCycleAll.end());
    sort(deleteCycleAll.begin(),deleteCycleAll.end());

    cout << "Algorithm=BTree" << ";Data=" << FILE_NAME_R << ";MatchRate=" << MATCH_RATE;
    cout << ";Fanout=" << FANOUT_BP << ";SplitError=" << 0 << ";TimeWindow=" << TIME_WINDOW << ";UpdateLength=" << (TEST_LEN/TIME_WINDOW)-1;
    cout << ";AvgSearchLatency=" << (reduce(searchCycleAll.begin(), searchCycleAll.end(), 0.0) / searchCycleAll.size())/machine_frequencey;
    cout << ";LastSearchLatency=" << (double)searchCycleAll.back()/machine_frequencey;
    cout << ";AvgInsertLatency=" << (reduce(insertCycleAll.begin(), insertCycleAll.end(), 0.0) / insertCycleAll.size())/machine_frequencey;
    cout << ";LastInsertLatency=" << (double)insertCycleAll.back()/machine_frequencey;
    cout << ";AvgDeleteLatency=" << (reduce(deleteCycleAll.begin(), deleteCycleAll.end(), 0.0) / deleteCycleAll.size())/machine_frequencey;
    cout << ";LastDeleteLatency=" << (double)deleteCycleAll.back()/machine_frequencey;
    cout << ";TotalTime=" << (double)totalCycle/machine_frequencey << ";";
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

    uint64_t maxTimestamp =  get<1>(data.back());
    srand(1); //When using searchTuple
    
    for(uint64_t i = startTime; i < maxTimestamp; i++ )
    {
        while(get<1>(*it) == i)
        {
            
            tuple<uint64_t,uint64_t,uint64_t> searchTuple = data.at((itDelete - data.begin()) + (rand() % ( (it - data.begin()) - (itDelete - data.begin()) + 1 )));
            pair<uint64_t,uint64_t> insertTuple = make_pair(get<0>(*it),get<1>(*it));

            uint64_t tempDeleteCycles = 0;
            startTimer(&tempDeleteCycles);
            carmi_erase(carmi,insertTuple);
            stopTimer(&tempDeleteCycles);
            deleteCycle += tempDeleteCycles;
            deleteCycleAll.push_back(tempDeleteCycles);

            uint64_t tempSearchCycles = 0;
            vector<pair<uint64_t, uint64_t>> tempJoinResult;
            startTimer(&tempSearchCycles);
            carmi_range_search(carmi,searchTuple,tempJoinResult);
            stopTimer(&tempSearchCycles);
            searchCycle += tempSearchCycles;
            searchCycleAll.push_back(tempSearchCycles);

            uint64_t tempInsertCycles = 0;
            startTimer(&tempInsertCycles);
            carmi.insert(insertTuple);
            stopTimer(&tempInsertCycles);
            insertCycle += tempInsertCycles;
            insertCycleAll.push_back(tempInsertCycles);

            it++;
        }
    }

    totalCycle = searchCycle + insertCycle + deleteCycle;

    sort(searchCycleAll.begin(),searchCycleAll.end());
    sort(insertCycleAll.begin(),insertCycleAll.end());
    sort(deleteCycleAll.begin(),deleteCycleAll.end());

    cout << "Algorithm=CARMI" << ";Data=" << FILE_NAME_R << ";MatchRate=" << MATCH_RATE;
    cout << ";Fanout=" << 0 << ";SplitError=" << 0 << ";TimeWindow=" << TIME_WINDOW << ";UpdateLength=" << (TEST_LEN/TIME_WINDOW)-1;
    cout << ";AvgSearchLatency=" << (reduce(searchCycleAll.begin(), searchCycleAll.end(), 0.0) / searchCycleAll.size())/machine_frequencey;
    cout << ";LastSearchLatency=" << (double)searchCycleAll.back()/machine_frequencey;
    cout << ";AvgInsertLatency=" << (reduce(insertCycleAll.begin(), insertCycleAll.end(), 0.0) / insertCycleAll.size())/machine_frequencey;
    cout << ";LastInsertLatency=" << (double)insertCycleAll.back()/machine_frequencey;
    cout << ";AvgDeleteLatency=" << (reduce(deleteCycleAll.begin(), deleteCycleAll.end(), 0.0) / deleteCycleAll.size())/machine_frequencey;
    cout << ";LastDeleteLatency=" << (double)deleteCycleAll.back()/machine_frequencey;
    cout << ";TotalTime=" << (double)totalCycle/machine_frequencey << ";";
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
            run_alex(data);
            break;
        case 1:
            run_btree(data);
            break;
        case 2:
            run_pgm(data);
            break;
        case 3:
            // run_carmi(data);
            break;
    }
    #else
    run_alex(data);
    run_pgm(data);
    run_btree(data);
    // run_carmi(data);
    #endif;

    return 0;
}