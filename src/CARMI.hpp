#pragma once
#include "../lib/carmi/carmi_map.h"
#include "../parameters.hpp"

using namespace std;

//Gives segmentation faults

template<class Type_Key, class Type_Ts>
inline void carmi_range_search(  CARMIMap<Type_Key,Type_Ts> & carmi,
                                pair<Type_Key, Type_Ts> & arrivalTuple, Type_Key & searchRange, vector<pair<Type_Key, Type_Ts>> & searchResult)
{
    Type_Key targetKey = arrivalTuple.first;
    Type_Ts newTimeStamp = arrivalTuple.second;
    
    Type_Key lowerBound = (double)targetKey - (double)searchRange < numeric_limits<Type_Key>::min()?
                           numeric_limits<Type_Key>::min() : targetKey - searchRange; 

    Type_Key upperBound = (double)targetKey + (double)searchRange > numeric_limits<Type_Key>::max()?
                           numeric_limits<Type_Key>::max() : targetKey + searchRange; 

    auto it = carmi.lower_bound(lowerBound);

    while (it != carmi.end() && it->first <= upperBound)
    { 
        if (it->first >= lowerBound)
        {
            searchResult.push_back(*it);
        }
        it++;
    }
}

template<class Type_Key, class Type_Ts>
inline void carmi_range_search(  CARMIMap<Type_Key,Type_Ts> & carmi,
                                pair<Type_Key, Type_Ts> & arrivalTuple, vector<pair<Type_Key, Type_Ts>> & searchResult)
{
    Type_Key targetKey = arrivalTuple.first;
    Type_Ts newTimeStamp = arrivalTuple.second;
    
    auto it = carmi.lower_bound(targetKey);

    while (it != carmi.end() && searchResult.size() < MATCH_RATE)
    { 
        searchResult.push_back(*it);
        it++;
    }
}

template<class Type_Key, class Type_Ts>
inline void carmi_range_search(  CARMIMap<Type_Key,Type_Ts> & carmi,
                                tuple<Type_Key, Type_Ts, Type_Key> & arrivalTuple, vector<pair<Type_Key, Type_Ts>> & searchResult)
{
    Type_Key targetKey = get<0>(arrivalTuple);
    Type_Ts newTimeStamp = get<1>(arrivalTuple);
    Type_Key upperBound = get<2>(arrivalTuple);

    auto it = carmi.lower_bound(targetKey);

    while (it != carmi.end() && it->first <= upperBound)
    { 
        if (it->first >= targetKey)
        {
            searchResult.push_back(*it);
        }
        it++;
    }
}

template<class Type_Key, class Type_Ts>
inline bool carmi_point_lookup(  CARMIMap<Type_Key,Type_Ts> & carmi,
                                pair<Type_Key, Type_Ts> & arrivalTuple)
{
    Type_Key targetKey = arrivalTuple.first;
    Type_Ts newTimeStamp = arrivalTuple.second;

    auto it = carmi.find(targetKey);

    if (it != carmi.end())
    {
        return true;
    }
    return false;
}
