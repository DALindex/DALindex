#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <random>
#include <cmath>
#include <fstream>

using namespace std;

template <typename Type_Key>
void print_temp_join_result(ofstream & foutRet, vector<pair<Type_Key, Type_Key>> & tempJoinResult)
{
    for (auto &it:tempJoinResult)
    {
        // cout << it.first << "," << it.second  << "(" <<  (int)it.first- (int)it.second << ")" << endl;
        foutRet <<it.first << "," << it.second  << "(" <<  (int)it.first- (int)it.second << ")" << endl;
        // foutRet << "R join S:" << it.first << "," << it.second  << "(" <<  (int)it.first- (int)it.second << ")" << endl;
    }
    //cout << "R:" << it_R->first << " " << it_R->second << endl;
}

template <typename Type_Key>
void print_temp_join_result(Type_Key newKey, ofstream & foutRet, vector<pair<Type_Key, Type_Key>> & tempJoinResult)
{
    for (auto &it:tempJoinResult)
    {
        // cout << "(" << it.first  << "," << it.second << ")," << newKey  << "[" <<  (int)it.first- (int)newKey << "]" << endl;
        foutRet << "(" << it.first  << "," << it.second << ")," << newKey  << "[" <<  (int)it.first- (int)newKey << "]" << endl;
        // foutRet << "R join S:" << "(" << it.first  << "," << it.second << ")," << newKey  << "[" <<  (int)it.first- (int)newKey << "]" << endl;
    }
    //cout << "R:" << "(" << it.first  << "," << it.second << ")," << newKey << endl;
}

template <typename Type_Key, typename Type_Ts>
void print_input_streams(vector<pair<Type_Key, Type_Ts>> & data_R, vector<pair<Type_Key, Type_Ts>> data_S)
{
    // for(auto it = data_R.begin(); it != data_R.end(); it++)
    // {
    //     cout << "(" << it->first << " " << it->second << ") ";
    // }
    // cout << endl;

    // for(auto it = data_S.begin(); it != data_S.end(); it++)
    // {
    //     cout << "(" << it->first << " " << it->second << ") ";
    // }
    // cout << endl;

    ofstream fout_R;
    fout_R.open("./benchmark/data_R.txt", ios::out);

    if (!fout_R.is_open())
    {
        cout << "Fail to open ./benchmark/data_R.txt\n" << endl;
    }

    for(auto &it:data_R)
    {
        fout_R << "(" << it.first << " " << it.second << ") \n";
    }
    fout_R.close();

    ofstream fout_S;
    
    fout_S.open("./benchmark/data_S.txt", ios::out);

    if (!fout_S.is_open())
    {
        cout << "Fail to open ./benchmark/data_S.txt\n" << endl;
    }

    for(auto &it:data_S)
    {
        fout_S << "(" << it.first << " " << it.second << ") \n";
    }
    fout_S.close();
}

template <typename Type_Key>
void print_vector_of_type_key(vector<Type_Key> v)
{
    cout << "[";
    for (auto &it:v)
    {
       cout << it << " "; 
    }
    cout << "]";
    
}

template <typename Type_Key, typename Type_Count>
void print_map_count(map<Type_Key,Type_Count> m)
{
    cout << "[";
    for (auto &it:m)
    {
       cout << it.first << " "; 
    }
    cout << "]";

    cout << ",Count:[";
    for (auto &it:m)
    {
       cout << it.second << " "; 
    }
    cout << "]";
    
}