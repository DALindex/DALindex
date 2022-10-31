#pragma once
#include "../lib/pgm_index_dynamic.hpp"
#include "helper.hpp"

using namespace std;

template<class Type_Key, class Type_Ts>
inline void pgm_range_search(  pgm::DynamicPGMIndex<Type_Key,Type_Ts,pgm::PGMIndex<Type_Key,FANOUT_BP>> & pgm,
                                pair<Type_Key, Type_Ts> & arrivalTuple, Type_Key & searchRange, vector<pair<Type_Key, Type_Ts>> & searchResult)
{
    Type_Key targetKey = arrivalTuple.first;
    Type_Ts newTimeStamp = arrivalTuple.second;
    
    Type_Key lowerBound = (double)targetKey - (double)searchRange < numeric_limits<Type_Key>::min()?
                           numeric_limits<Type_Key>::min() : targetKey - searchRange; 

    Type_Key upperBound = (double)targetKey + (double)searchRange > numeric_limits<Type_Key>::max()?
                           numeric_limits<Type_Key>::max() : targetKey + searchRange; 

    searchResult = pgm.range(lowerBound,upperBound);
}

template<class Type_Key, class Type_Ts>
inline void pgm_range_search(  pgm::DynamicPGMIndex<Type_Key,Type_Ts,pgm::PGMIndex<Type_Key,FANOUT_BP>> & pgm,
                                pair<Type_Key, Type_Ts> & arrivalTuple, vector<pair<Type_Key, Type_Ts>> & searchResult)
{
    Type_Key targetKey = arrivalTuple.first;
    Type_Ts newTimeStamp = arrivalTuple.second;
    
    auto it = pgm.lower_bound(targetKey);

    while (it != pgm.end() && searchResult.size() < MATCH_RATE)
    {
        searchResult.push_back(make_pair(it->first,it->second));
        ++it;
    }
}

template<class Type_Key, class Type_Ts>
inline void pgm_range_search(  pgm::DynamicPGMIndex<Type_Key,Type_Ts,pgm::PGMIndex<Type_Key,FANOUT_BP>> & pgm,
                                tuple<Type_Key, Type_Ts, Type_Key> & arrivalTuple, vector<pair<Type_Key, Type_Ts>> & searchResult)
{
    Type_Key targetKey = get<0>(arrivalTuple);
    Type_Ts newTimeStamp = get<1>(arrivalTuple);
    Type_Key upperBound = get<2>(arrivalTuple);

    searchResult = pgm.range(targetKey,upperBound);
}

template<class Type_Key, class Type_Ts>
inline bool pgm_point_lookup(  pgm::DynamicPGMIndex<Type_Key,Type_Ts,pgm::PGMIndex<Type_Key,FANOUT_BP>> & pgm,
                                pair<Type_Key, Type_Ts> & arrivalTuple)
{
    Type_Key targetKey = arrivalTuple.first;
    Type_Ts newTimeStamp = arrivalTuple.second;

    auto it = pgm.find(targetKey);

    if (it != pgm.end())
    {
        return true;
    }
    return false;
}
