#pragma once
#include "../lib/btree_map.h"
#include "helper.hpp"

using namespace std;

template<class Type_Key, class Type_Ts>
inline void bt_range_search(  stx::btree_map<Type_Key,Type_Ts,less<Type_Key>,btree_traits_fanout<Type_Key>> & btree,
                                pair<Type_Key, Type_Ts> & arrivalTuple, Type_Key & searchRange, vector<pair<Type_Key, Type_Ts>> & searchResult)
{
    Type_Key targetKey = arrivalTuple.first;
    Type_Ts newTimeStamp = arrivalTuple.second;
    
    Type_Key lowerBound = (double)targetKey - (double)searchRange < numeric_limits<Type_Key>::min()?
                           numeric_limits<Type_Key>::min() : targetKey - searchRange; 

    Type_Key upperBound = (double)targetKey + (double)searchRange > numeric_limits<Type_Key>::max()?
                           numeric_limits<Type_Key>::max() : targetKey + searchRange; 

    auto it = btree.lower_bound(lowerBound);

    while (it != btree.end() && it->first <= upperBound)
    { 
        if (it->first >= lowerBound)
        {
            searchResult.push_back(*it);
        }
        it++;
    }
}

template<class Type_Key, class Type_Ts>
inline void bt_range_search(  stx::btree_map<Type_Key,Type_Ts,less<Type_Key>,btree_traits_fanout<Type_Key>> & btree,
                                pair<Type_Key, Type_Ts> & arrivalTuple, vector<pair<Type_Key, Type_Ts>> & searchResult)
{
    Type_Key targetKey = arrivalTuple.first;
    Type_Ts newTimeStamp = arrivalTuple.second;
    
    auto it = btree.lower_bound(targetKey);

    while (it != btree.end() && searchResult.size() < MATCH_RATE)
    { 
        searchResult.push_back(*it);
        it++;
    }
}

template<class Type_Key, class Type_Ts>
inline void bt_range_search(  stx::btree_map<Type_Key,Type_Ts,less<Type_Key>,btree_traits_fanout<Type_Key>> & btree,
                                tuple<Type_Key, Type_Ts, Type_Key> & arrivalTuple, vector<pair<Type_Key, Type_Ts>> & searchResult)
{
    Type_Key targetKey = get<0>(arrivalTuple);
    Type_Ts newTimeStamp = get<1>(arrivalTuple);
    Type_Key upperBound = get<2>(arrivalTuple);

    auto it = btree.lower_bound(targetKey);

    while (it != btree.end() && it->first <= upperBound)
    { 
        if (it->first >= targetKey)
        {
            searchResult.push_back(*it);
        }
        it++;
    }
}

template<class Type_Key, class Type_Ts>
inline bool bt_point_lookup(  stx::btree_map<Type_Key,Type_Ts,less<Type_Key>,btree_traits_fanout<Type_Key>> & btree,
                                pair<Type_Key, Type_Ts> & arrivalTuple)
{
    Type_Key targetKey = arrivalTuple.first;
    Type_Ts newTimeStamp = arrivalTuple.second;

    auto it = btree.find(targetKey);

    if (it != btree.end())
    {
        return true;
    }
    return false;
}
