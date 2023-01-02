#pragma once
 
#include<iostream>
#include<algorithm>
#include<iterator>
#include<vector>
#include<array>
#include<numeric>
#include<cassert>

#include <x86intrin.h>
#include <bitset>
#include <stdint.h>

#include "config.hpp"
#include "../parameters.hpp"

using namespace std;

/*
STX B+Tree Parameters
*/
template<typename Type>
class btree_traits_fanout{
public:
    static const bool selfverify = false;
    static const bool debug = false;
    static const int leafslots = FANOUT_BP;
    static const int innerslots = FANOUT_BP;
    static const size_t binsearch_threshold = 256;
};

/*
Function Headers
*/
template<class Type_Key, class Type_Ts>
void calculate_split_derivative(int noSeg, vector<pair<Type_Key,Type_Ts>> & data, vector<tuple<int,int,double>> & splitIndexSlopeVector);

template<class Type_Key, class Type_Ts>
void calculate_split_equal(int noSeg, vector<pair<Type_Key,Type_Ts>> & data, vector<tuple<int,int,double>> & splitIndexSlopeVector);

template<class Type_Key, class Type_Ts>
void calculate_split_index_shrinking_cone(vector<pair<Type_Key,Type_Ts>> & data, vector<tuple<int,int,double>> &splitIndexSlopeVector);

template<class Type_Key, class Type_Ts>
void calculate_split_index_shrinking_cone(vector<pair<Type_Key,Type_Ts>> & data, vector<tuple<int,int,double>> &splitIndexSlopeVector, int & splitError);

template<class Type_Key, class Type_Ts>
void calculate_split_index_shrinking_cone_least_sqaure(vector<pair<Type_Key,Type_Ts>> & data, vector<tuple<int,int,double>> &splitIndexSlopeVector, int & splitError);

template<class Type_Key, class Type_Ts>
void calculate_split_index_shrinking_cone(vector<Type_Key> & data, vector<tuple<int,int,double>> &splitIndexSlopeVector, int & splitError);

template<class Type_Key, class Type_Ts>
void calculate_split_derivative(int noSeg, vector<pair<Type_Key,Type_Ts>> & data, vector<tuple<int,int,double>> & splitIndexSlopeVector)
{
    #ifdef DEBUG
    cout << "[Debug Info:] Util Function {calculate_split_index_vect(noSeg, data, slplitIndexVector)} Begin" << endl;
    cout << "[Debug Info:] size of data = " << data.size() << endl;
    cout << endl;
    #endif

    vector<double> secondDerivative(data.size());
    secondDerivative[0] = 0;
    secondDerivative.back() = 0;
    for (int i = 2; i < data.size(); i++)
    {
        secondDerivative[i-1] =  abs((double)(data[i].first-data[i-1].first) - (data[i-1].first-data[i-2].first));      
    }

    vector<int> indexVec(data.size());
    iota(indexVec.begin(),indexVec.end(),0);
    sort(indexVec.begin(),indexVec.end(), [&](double i,double j){return secondDerivative[i] > secondDerivative[j];} );

    int slpitTimes = 0;
    vector<bool> splitIndexVector(data.size());
    splitIndexVector.back() = true;

    int halfSegSize = data.size()/(noSeg*2);

    for(int i = 0; i < indexVec.size(); i++)
    {        
        if(slpitTimes >= noSeg-1 || secondDerivative[indexVec[i]] <= 0)
        {
            break;
        }

        if( secondDerivative[indexVec[i]] > 0)
        {
            if (secondDerivative[indexVec[i+1]] < secondDerivative[indexVec[i-1]])
            {
                splitIndexVector[indexVec[i-1]] = true;
            }
            else
            {
                splitIndexVector[indexVec[i]] = true;
            }

            int tempIndex = indexVec[i]-1;
            while (tempIndex >= 0 && tempIndex >= indexVec[i]-halfSegSize)
            {
                secondDerivative[tempIndex--] = -1;
            }

            tempIndex = indexVec[i]+1;
            while (tempIndex < data.size() && tempIndex <= indexVec[i]+halfSegSize)
            {
                secondDerivative[tempIndex++] = -1;
            }

            slpitTimes++;
        
        }
    }

    int startSplitIndex = 0, cnt = 0;
    double sumKey = 0,sumIndex = 0, sumKeyIndex = 0, sumKeySquared = 0;
    for (int i = 0; i < data.size(); i++)
    {
        sumKey += (double)data[i].first;
        sumIndex += cnt;
        sumKeyIndex += (double)data[i].first * cnt;
        sumKeySquared += (double)pow(data[i].first,2);
        cnt++;
        
        if(splitIndexVector[i])
        {
            splitIndexSlopeVector.push_back(make_tuple(startSplitIndex,i,(sumKeyIndex - sumKey *(sumIndex/cnt))/(sumKeySquared - sumKey*(sumKey/cnt))));

            sumKey = 0;
            sumIndex = 0;
            sumKeyIndex = 0;
            sumKeySquared = 0;
            cnt = 0;
            startSplitIndex = i+1;
        }
    }

    #ifdef DEBUG
    cout << "[Debug Info:] Printing slplitIndexVector : " << endl;
    for (auto &it: splitIndexSlopeVector)
    {
        cout << "start = " << get<0>(it) << ", end = " << get<1>(it) << ", slope = " << get<2>(it) << endl;
    }
    cout << endl;
    #endif

    #ifdef DEBUG
    cout << "[Debug Info:] Util Function {calculate_split_index_vect(noSeg, data, slplitIndexVector)} End" << endl;
    #endif
}

template<class Type_Key, class Type_Ts>
void calculate_split_equal(int noSeg, vector<pair<Type_Key,Type_Ts>> & data, vector<tuple<int,int,double>> & splitIndexSlopeVector)
{
    #ifdef DEBUG
    cout << "[Debug Info:] Util Function {calculate_split_index_vect(noSeg, data, slplitIndexVector)} Begin" << endl;
    cout << "[Debug Info:] size of data = " << data.size() << endl;
    cout << endl;
    #endif

    int segSize = data.size()/(noSeg);
    vector<bool> splitIndexVector(data.size());
    
    int startIndex = 0;
    for (int i = 0; i < noSeg-1; i++)
    {
        splitIndexVector[startIndex] = true;
        splitIndexVector[startIndex+segSize-1] = true;
        startIndex = startIndex+segSize;
    }

    if (startIndex >= data.size()-1)
    {
        splitIndexVector.back() = true;
    }
    else
    {
        splitIndexVector[startIndex] = true;
        splitIndexVector.back() = true;
    }

    int startSplitIndex = 0, cnt = 0;
    double sumKey = 0,sumIndex = 0, sumKeyIndex = 0, sumKeySquared = 0;
    for (int i = 0; i < data.size(); i++)
    {
        sumKey += (double)data[i].first;
        sumIndex += cnt;
        sumKeyIndex += (double)data[i].first * cnt;
        sumKeySquared += (double)pow(data[i].first,2);
        cnt++;
        
        if(splitIndexVector[i])
        {
            splitIndexSlopeVector.push_back(make_tuple(startSplitIndex,i,(sumKeyIndex - sumKey *(sumIndex/cnt))/(sumKeySquared - sumKey*(sumKey/cnt))));

            sumKey = 0;
            sumIndex = 0;
            sumKeyIndex = 0;
            sumKeySquared = 0;
            cnt = 0;
            startSplitIndex = i+1;
        }
    }

    #ifdef DEBUG
    cout << "[Debug Info:] Printing slplitIndexVector : " << endl;
    for (auto &it: splitIndexSlopeVector)
    {
        cout << "start = " << get<0>(it) << ", end = " << get<1>(it) << ", slope = " << get<2>(it) << endl;
    }
    cout << endl;
    #endif

    #ifdef DEBUG
    cout << "[Debug Info:] Util Function {calculate_split_index_vect(noSeg, data, slplitIndexVector)} End" << endl;
    #endif
}


template<class Type_Key, class Type_Ts>
void calculate_split_index_shrinking_cone(vector<pair<Type_Key,Type_Ts>> & data, vector<tuple<int,int,double>> &splitIndexSlopeVector)
{
    #ifdef DEBUG
    cout << "[Debug Info:] Util Function {calculate_split_index_shrinking_cone(data, slplitIndexSlope)} Start" << endl;
    cout << "[Debug Info:] size of DualData = " << data.size() << endl;
    cout << endl;
    #endif

    double slopeLow = 0;
    double slopeHigh = numeric_limits<double>::max();
    double slope = 1;

    double keyStart = data.front().first;
    
    int index = 1;
    int splitIndexStart = 0; 
    int splitIndexEnd = 1;

    for (auto it = data.begin()+1; it != data.end(); it++)
    {
        double denominator = it->first - keyStart;

        if (slopeLow <= (double)(index) / denominator &&
		    (double)(index) / denominator <= slopeHigh) 
        {
            #ifdef DEBUG
            double slopeHighTemp = ((double)(index + GLOBAL_ERROR)) / denominator;
 		    double slopeLowTemp = ((double)(index - GLOBAL_ERROR)) / denominator;
            #endif
            
            slopeHigh = min(slopeHigh,(((double)(index + GLOBAL_ERROR)) / denominator));
            slopeLow = max(slopeLow, (((double)(index - GLOBAL_ERROR)) / denominator));
            slope = 0.5*(slopeHigh + slopeLow);

            index++;
        }
        else
        {
            splitIndexSlopeVector.push_back(make_tuple(splitIndexStart,splitIndexEnd-1,slope));
            
            splitIndexStart = splitIndexEnd;
            index = 1;
            keyStart = it->first;
            slopeLow = 0;
            slopeHigh = numeric_limits<double>::max();
            slope = 1;
        }

        splitIndexEnd++;
    }

    splitIndexSlopeVector.push_back(make_tuple(splitIndexStart,splitIndexEnd-1,slope));

    #ifdef DEBUG
    cout << "[Debug Info:] Util Function {calculate_split_index_shrinking_cone(data, slplitIndexSlope)} End" << endl;
    cout << "Shrinking Cone Split:" << endl;
    for (auto &it: splitIndexSlopeVector)
    {
        cout << "start = " << get<0>(it) << ", end = " << get<1>(it) << ", slope = " << get<2>(it) << endl;
    }
    cout <<endl;
    #endif
}

template<class Type_Key, class Type_Ts>
void calculate_split_index_shrinking_cone(vector<pair<Type_Key,Type_Ts>> & data, vector<tuple<int,int,double>> &splitIndexSlopeVector, int & splitError)
{
    #ifdef DEBUG
    cout << "[Debug Info:] Util Function {calculate_split_index_shrinking_cone(data, slplitIndexSlope, splitError)} Start" << endl;
    cout << "[Debug Info:] size of DualData = " << data.size() << endl;
    cout << endl;
    #endif

    double slopeLow = 0;
    double slopeHigh = numeric_limits<double>::max();
    double slope = 1;

    double keyStart = data.front().first;
    
    int index = 1;
    int splitIndexStart = 0; 
    int splitIndexEnd = 1;

    for (auto it = data.begin()+1; it != data.end(); it++)
    {
        double denominator = it->first - keyStart;

        if (slopeLow <= (double)(index) / denominator &&
		    (double)(index) / denominator <= slopeHigh) 
        {
            #ifdef DEBUG
            double slopeHighTemp = ((double)(index + GLOBAL_ERROR)) / denominator;
 		    double slopeLowTemp = ((double)(index - GLOBAL_ERROR)) / denominator;
            #endif
            
            slopeHigh = min(slopeHigh,(((double)(index + splitError)) / denominator));
            slopeLow = max(slopeLow, (((double)(index - splitError)) / denominator));
            slope = 0.5*(slopeHigh + slopeLow);

            index++;
        }
        else
        {
            splitIndexSlopeVector.push_back(make_tuple(splitIndexStart,splitIndexEnd-1,slope));
            
            splitIndexStart = splitIndexEnd;
            index = 1;
            keyStart = it->first;
            slopeLow = 0;
            slopeHigh = numeric_limits<double>::max();
            slope = 1;
        }

        splitIndexEnd++;
    }

    splitIndexSlopeVector.push_back(make_tuple(splitIndexStart,splitIndexEnd-1,slope));

    #ifdef DEBUG
    cout << "[Debug Info:] Util Function {calculate_split_index_shrinking_cone(data, slplitIndexSlope, splitError)} End" << endl;
    cout << "Shrinking Cone Split:" << endl;
    for (auto &it: splitIndexSlopeVector)
    {
        cout << "start = " << get<0>(it) << ", end = " << get<1>(it) << ", slope = " << get<2>(it) << endl;
    }
    cout <<endl;
    #endif
}

template<class Type_Key, class Type_Ts>
void calculate_split_index_shrinking_cone_least_sqaure(vector<pair<Type_Key,Type_Ts>> & data, vector<tuple<int,int,double>> &splitIndexSlopeVector, int & splitError)
{
    #ifdef DEBUG
    cout << "[Debug Info:] Util Function {calculate_split_index_shrinking_cone_least_sqaure(data, slplitIndexSlope, splitError)} Start" << endl;
    cout << "[Debug Info:] size of DualData = " << data.size() << endl;
    cout << endl;
    #endif

    double slopeLow = 0;
    double slopeHigh = numeric_limits<double>::max();
    double slope = 1;

    double keyStart = data.front().first;
    
    int index = 1;
    int splitIndexStart = 0; 
    int splitIndexEnd = 1;

    double sumKey = (double)keyStart,sumIndex = 1, sumKeyIndex = 0, sumKeySquared = (double)pow(keyStart,2);
    for (auto it = data.begin()+1; it != data.end(); it++)
    {
        double denominator = it->first - keyStart;

        if (slopeLow <= (double)(index) / denominator &&
		    (double)(index) / denominator <= slopeHigh) 
        {
            #ifdef DEBUG
            double slopeHighTemp = ((double)(index + GLOBAL_ERROR)) / denominator;
 		    double slopeLowTemp = ((double)(index - GLOBAL_ERROR)) / denominator;
            #endif
            
            slopeHigh = min(slopeHigh,(((double)(index + splitError)) / denominator));
            slopeLow = max(slopeLow, (((double)(index - splitError)) / denominator));

            sumKey += (double)it->first;
            sumIndex += index;
            sumKeyIndex += (double)it->first * index;
            sumKeySquared += (double)pow(it->first,2);
            slope = (sumKeyIndex - sumKey *(sumIndex/index))/(sumKeySquared - sumKey*(sumKey/index));

            index++;
        }
        else
        {            
            splitIndexSlopeVector.push_back(make_tuple(splitIndexStart,splitIndexEnd-1,slope));
            
            splitIndexStart = splitIndexEnd;

            index = 1;
            keyStart = it->first;
            slopeLow = 0;
            slopeHigh = numeric_limits<double>::max();

            sumKey = (double)it->first;
            sumIndex = 0;
            sumKeyIndex = 0;
            sumKeySquared = (double)pow(it->first,2);
            slope = 1;
        }

        splitIndexEnd++;
    }
    
    splitIndexSlopeVector.push_back(make_tuple(splitIndexStart,splitIndexEnd-1,slope));

    #ifdef DEBUG
    cout << "[Debug Info:] Util Function {calculate_split_index_shrinking_cone_least_sqaure(data, slplitIndexSlope, splitError)} End" << endl;
    cout << "Shrinking Cone Split:" << endl;
    for (auto &it: splitIndexSlopeVector)
    {
        cout << "start = " << get<0>(it) << ", end = " << get<1>(it) << ", slope = " << get<2>(it) << endl;
    }
    cout <<endl;
    #endif
}

template<class Type_Key, class Type_Ts>
void calculate_split_index_shrinking_cone(vector<Type_Key> & data, vector<tuple<int,int,double>> &splitIndexSlopeVector, int & splitError)
{
    #ifdef DEBUG
    cout << "[Debug Info:] Util Function {calculate_split_index_shrinking_cone(data, slplitIndexSlope, splitError)} Start" << endl;
    cout << "[Debug Info:] size of DualData = " << data.size() << endl;
    cout << endl;
    #endif

    double slopeLow = 0;
    double slopeHigh = numeric_limits<double>::max();
    double slope = 1;

    double keyStart = data.front().first;
    
    int index = 1;
    int splitIndexStart = 0; 
    int splitIndexEnd = 1;

    for (auto it = data.begin()+1; it != data.end(); it++)
    {
        double denominator = *it - keyStart;

        if (slopeLow <= (double)(index) / denominator &&
		    (double)(index) / denominator <= slopeHigh) 
        {
            #ifdef DEBUG
            double slopeHighTemp = ((double)(index + GLOBAL_ERROR)) / denominator;
 		    double slopeLowTemp = ((double)(index - GLOBAL_ERROR)) / denominator;
            #endif
            
            slopeHigh = min(slopeHigh,(((double)(index + splitError)) / denominator));
            slopeLow = max(slopeLow, (((double)(index - splitError)) / denominator));
            slope = 0.5*(slopeHigh + slopeLow);

            index++;
        }
        else
        {
            splitIndexSlopeVector.push_back(make_tuple(splitIndexStart,splitIndexEnd-1,slope));
            
            splitIndexStart = splitIndexEnd;
            index = 1;
            keyStart = *it;
            slopeLow = 0;
            slopeHigh = numeric_limits<double>::max();
            slope = 1;
        }

        splitIndexEnd++;
    }

    splitIndexSlopeVector.push_back(make_tuple(splitIndexStart,splitIndexEnd-1,slope));

    #ifdef DEBUG
    cout << "[Debug Info:] Util Function {calculate_split_index_shrinking_cone(data, slplitIndexSlope, splitError)} End" << endl;
    cout << "Shrinking Cone Split:" << endl;
    for (auto &it: splitIndexSlopeVector)
    {
        cout << "start = " << get<0>(it) << ", end = " << get<1>(it) << ", slope = " << get<2>(it) << endl;
    }
    cout <<endl;
    #endif
}



/*
Bitmap Helpers
from ALEX (https://github.dev/microsoft/ALEX)
*/

//extract_rightmost_one(010100100) = 000000100  or if value = 10 (1010) -> extracts 2 (0010)
inline uint64_t bit_extract_rightmost_bit(uint64_t value)
{
    return value & -static_cast<int64_t>(value);
}

//remove_rightmost_one(010100100) = 010100000
inline uint64_t bit_remove_rightmost_one(uint64_t value) {
  return value & (value - 1);
}

//count_ones(010100100) = 3
inline int bit_count_ones(uint64_t value) {
  return static_cast<int>(_mm_popcnt_u64(value));
}

//Get index of bit in bitmap
//bitmapPos is the position of the current bitmap in the vector/array of bitmap
//value is the an uin64_t where the bit at the target position is 1 while the rest is 0. (i.e. 000000100000 if bit at index 6 is non-gap)
//value is usually a product of extract_rightmost_bit() (i.e. get_index(bitmapPos,extract_rightmost_bit(bitmap[bitmapPos])));
inline int bit_get_index(int bitmapPos, uint64_t value) {
  return (bitmapPos << 6) + bit_count_ones(value - 1);
}
