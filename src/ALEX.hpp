#pragma once
#include "../lib/alex.h"
#include "../parameters.hpp"

using namespace std;

template<class Type_Key, class Type_Ts>
inline void alex_erase( alex::Alex<Type_Key,Type_Ts> & alex, 
                        pair<Type_Key, Type_Ts> & arrivalTuple)
{
    vector<Type_Key> deleteKeys;
    deleteKeys.reserve(1000); //Predefined size to prevent vector extending
    Type_Ts lowerLimit =  ((double)arrivalTuple.second - TIME_WINDOW < numeric_limits<Type_Ts>::min()) 
                            ? numeric_limits<Type_Ts>::min(): arrivalTuple.second - TIME_WINDOW;

    auto itStart = alex.begin();
    auto itEnd = alex.end();
    while (itStart != itEnd)
    {
        if (itStart.payload() < lowerLimit)
        {
            deleteKeys.push_back(itStart.key());
        }
        itStart++;
    }

    for (auto &key:deleteKeys)
    {
        alex.erase(key);
    }
}

template<class Type_Key, class Type_Ts>
inline void alex_range_search(  alex::Alex<Type_Key,Type_Ts> & alex,
                                pair<Type_Key, Type_Ts> & arrivalTuple, Type_Key & searchRange, vector<pair<Type_Key, Type_Ts>> & searchResult)
{
    Type_Key targetKey = arrivalTuple.first;
    Type_Ts newTimeStamp = arrivalTuple.second;
    
    Type_Key lowerBound = (double)targetKey - (double)searchRange < numeric_limits<Type_Key>::min()?
                           numeric_limits<Type_Key>::min() : targetKey - searchRange; 

    Type_Key upperBound = (double)targetKey + (double)searchRange > numeric_limits<Type_Key>::max()?
                           numeric_limits<Type_Key>::max() : targetKey + searchRange; 

    auto itStart = alex.lower_bound(lowerBound);
    auto itEnd = alex.lower_bound(upperBound);

    while (itStart != itEnd)
    {
        if (itStart.key() >= lowerBound && itStart.key() <= upperBound)
        {
            searchResult.push_back(make_pair(itStart.key(), itStart.payload()));
            // if (itStart.payload() >= newTimeStamp - TIME_WINDOW)
            // {
            //     // searchResult.push_back(*itAlex);
            //     searchResult.push_back(make_pair(itStart.key(), itStart.payload()));
            // }
        }
        itStart++;
    }
}

template<class Type_Key, class Type_Ts>
inline void alex_range_search(  alex::Alex<Type_Key,Type_Ts> & alex,
                                pair<Type_Key, Type_Ts> & arrivalTuple, vector<pair<Type_Key, Type_Ts>> & searchResult)
{
    Type_Key targetKey = arrivalTuple.first;
    Type_Ts newTimeStamp = arrivalTuple.second;
    
    auto it = alex.lower_bound(targetKey);

    while (searchResult.size() < MATCH_RATE && it != alex.end())
    {
        searchResult.push_back(make_pair(it.key(), it.payload()));
        // if (it.payload() >= newTimeStamp - TIME_WINDOW)
        // {
        //     searchResult.push_back(make_pair(it.key(), it.payload()));
        // }
        it++;
    }
}

template<class Type_Key, class Type_Ts>
inline void alex_range_search(  alex::Alex<Type_Key,Type_Ts> & alex,
                                tuple<Type_Key, Type_Ts, Type_Key> & arrivalTuple, vector<pair<Type_Key, Type_Ts>> & searchResult)
{
    Type_Key targetKey = get<0>(arrivalTuple);
    Type_Ts newTimeStamp = get<1>(arrivalTuple);
    Type_Key upperBound = get<2>(arrivalTuple);

    auto itStart = alex.lower_bound(targetKey);
    auto itEnd = alex.lower_bound(upperBound);

    while (itStart != itEnd)
    {
        if (itStart.key() >= targetKey && itStart.key() <= upperBound)
        {
            searchResult.push_back(make_pair(itStart.key(), itStart.payload()));
            // if (itStart.payload() >= newTimeStamp - TIME_WINDOW)
            // {
            //     // searchResult.push_back(*itAlex);
            //     searchResult.push_back(make_pair(itStart.key(), itStart.payload()));
            // }
        }
        itStart++;
    }
}

template<class Type_Key, class Type_Ts>
inline bool alex_point_lookup(  alex::Alex<Type_Key,Type_Ts> & alex,
                                pair<Type_Key, Type_Ts> & arrivalTuple)
{
    Type_Key targetKey = arrivalTuple.first;
    Type_Ts newTimeStamp = arrivalTuple.second;

    auto it = alex.find(targetKey);

    if (it != alex.end())
    {
        return true;
    }
    return false;
}

template<class Type_Key, class Type_Ts>
inline uint64_t alex_get_total_size_in_bytes(  alex::Alex<Type_Key,Type_Ts> & alex)
{
   return static_cast<uint64_t>(alex.model_size()) + static_cast<uint64_t>(alex.data_size());
}