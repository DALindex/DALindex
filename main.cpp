#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <random>
#include <cmath>
#include <fstream>

#include "src/DALroot.hpp"
#include "src/DALtuner.hpp"
#include "utils/load_with_timestamp.cpp"

using namespace std;

// string data_dir = "/data/"; //Place Holders
string data_dir =  "/data/Documents/data/";
string data_file = "f_books"; //Place Holders
int matchRate = 1000;
int seed = 10;

void load_data(vector<tuple<uint64_t, uint64_t, uint64_t>> & data)
{
    string input_file = data_dir+data_file;
    add_timestamp(input_file, data, matchRate ,seed);
}

int main()
{
    //load data
    vector<tuple<uint64_t, uint64_t, uint64_t>> data;
    data.reserve(TEST_LEN);
    load_data(data);

    //bulk load initial data into DALindex
    vector<pair<uint64_t, uint64_t>> data_initial;
    data_initial.reserve(TIME_WINDOW);
    for (auto it = data.begin(); it != data.begin()+TIME_WINDOW; it++)
    {
        data_initial.push_back(make_pair(get<0>(*it),get<1>(*it)));
    }
    DALroot<uint64_t,uint64_t> DALindex(data_initial);

    //Lookup
    pair<uint64_t,uint64_t> searchPair = make_pair(get<0>(data[int(TIME_WINDOW/2)]), get<0>(data[int(TIME_WINDOW/2)]));
    DALindex.tree_lookup(searchPair);

    //Range Search
    tuple<uint64_t,uint64_t,uint64_t> searchTuple = data[int(TIME_WINDOW/2)];
    vector<pair<uint64_t, uint64_t>> searchResult;
    DALindex.tree_range_search(searchTuple,searchResult);

    //Insert
    pair<uint64_t,uint64_t> insertPair = make_pair(get<0>(data[int(TIME_WINDOW+10)]), get<0>(data[int(TIME_WINDOW+10)]));
    DALindex.tree_insert(insertPair);

    return 0;
}