#pragma once

#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <array>
#include <cmath>

#include "helper.hpp"

#if defined(DEBUG) || defined(DEBUG_KEY)
#include "../utils/debug.hpp"
#endif

#ifdef TUNE
#include "../utils/tuner_variables.hpp"
uint64_t tuneStage = 0;

uint64_t rootNoExponentialSearch = 0;
uint64_t rootLengthExponentialSearch = 0;

uint64_t segNoExponentialSearch = 0;
uint64_t segLengthExponentialSearch = 0;

uint64_t segNoScan = 0;
uint64_t segScanNoSeg = 0;

uint64_t segNoRetrain = 0;
uint64_t segLengthMerge = 0;
uint64_t segLengthRetrain = 0;

uint64_t segNoExponentialSearchAll = 0;
uint64_t segLengthExponentialSearchAll = 0;
#endif

#ifdef TUNE_TIME
#include "../utils/tuner_time_variables.hpp"
uint64_t rootNoExponentialSearch = 0;
uint64_t rootLengthExponentialSearch = 0;

uint64_t segNoExponentialSearch = 0;
uint64_t segLengthExponentialSearch = 0;

uint64_t segNoScan = 0;
uint64_t segScanNoSeg = 0;

uint64_t segNoRetrain = 0;
uint64_t segLengthMerge = 0;
uint64_t segLengthRetrain = 0;

uint64_t segNoExponentialSearchAll = 0;
uint64_t segLengthExponentialSearchAll = 0;

uint64_t rootNoSearch = 0;
uint64_t rootNoPredict = 0;
uint64_t rootPredictCycle = 0;
uint64_t rootExponentialSearchCycle = 0;
double rootExponentialSearchCyclePerLength = 0;

uint64_t segNoSearch = 0;
uint64_t segNoPredict = 0;
uint64_t segPredictCycle = 0;
uint64_t segExponentialSearchCycle = 0;
double segExponentialSearchCyclePerLength = 0;

uint64_t bufferNoSearch = 0;
uint64_t bufferLengthBinarySearch = 0;
uint64_t bufferBinarySearchCycle = 0;
double bufferBinarySearchCyclePerLength = 0;

uint64_t rootInsertCycle = 0;
double rootInsertCyclePerLength = 0;
uint64_t rootNoInsert = 0;
uint64_t rootSizeInsert = 0;

uint64_t segNoInsert = 0;
uint64_t segInsertCycle = 0;

uint64_t rootNoDeletion = 0;
uint64_t rootDeletionCycle = 0;

uint64_t rootNoRetrain = 0;
uint64_t rootLengthRetrain = 0;
uint64_t rootRetrainCycle = 0;
double rootRetrainCyclePerLength = 0;

uint64_t segMergeCycle = 0;
double  segMergeCyclePerLength = 0;
uint64_t segRetrainCycle = 0;
double segRetrainCyclePerLength = 0;

uint64_t searchCycle = 0;
uint64_t insertCycle = 0;
uint64_t scanCycle = 0;
uint64_t retrainCycle = 0;
#endif

#ifdef TIME_BREAKDOWN
#include "../utils/timer_variables.hpp"
uint64_t searchCycle = 0;
uint64_t insertCycle = 0;
uint64_t scanCycle = 0;
uint64_t retrainCycle = 0;
#endif


using namespace std;

template<class Type_Key, class Type_Ts> class DALroot;

template<class Type_Key, class Type_Ts>
class DALseg
{
private:
    int m_parentIndex;
    int m_rightSearchBound;
    int m_leftSearchBound;
    int m_numPair;
    int m_numPairExist;
    int m_numPairBuffer;
    int m_maxSearchError;
    int m_tuneStage;

    Type_Ts m_maxTimeStamp;
    double m_slope;

    Type_Key m_startKey; // first key of DP
    Type_Key m_currentNodeStartKey; // existing first key of seg/node

    vector<pair<Type_Key,Type_Ts>> m_buffer;
    vector<pair<Type_Key,Type_Ts>> m_localData;

    DALseg<Type_Key,Type_Ts> * m_leftSibling = nullptr;
    DALseg<Type_Key,Type_Ts> * m_rightSibling = nullptr;
    

public:
    DALseg(int startSplitIndex, int endSplitIndex, vector<pair<Type_Key,Type_Ts>> & data)
    :m_rightSearchBound(0), m_leftSearchBound(0), m_numPair(0), m_numPairBuffer(0), m_maxTimeStamp(0), m_parentIndex(0), m_maxSearchError(0)
    {
        #ifdef DEBUG
        cout << "[Debug Info:] Class {DALseg} :: Construct Function {DALseg(startSplitIndex, endSplitIndex, data)} Begin" << endl;
        cout << endl;
        #endif

        m_buffer.reserve(MAX_BUFFER_SIZE);
        local_train_calculate_slope(startSplitIndex, endSplitIndex, data);

        #ifdef TUNE
        m_tuneStage = tuneStage;
        #endif

        #ifdef DEBUG
        cout << "[Debug Info:] Class {DALseg} :: Construct Function {DALseg(startSplitIndex, endSplitIndex, data)} End" << endl;
        cout << endl;
        #endif
    }

    DALseg(int startSplitIndex, int endSplitIndex, double slope, vector<pair<Type_Key,Type_Ts>> & data)
    :m_rightSearchBound(0), m_leftSearchBound(0), m_numPair(0), m_numPairBuffer(0), m_maxTimeStamp(0), m_parentIndex(0), m_maxSearchError(0)
    {
        #ifdef DEBUG
        cout << "[Debug Info:] Class {DALseg} :: Construct Function {DALseg(startSplitIndex, endSplitIndex, slope, data)} Begin" << endl;
        cout << endl;
        #endif

        m_buffer.reserve(MAX_BUFFER_SIZE);
        local_train(startSplitIndex, endSplitIndex, slope, data);

        #ifdef TUNE
        m_tuneStage = tuneStage;
        #endif

        #ifdef DEBUG
        cout << "[Debug Info:] Class {DALseg} :: Construct Function {DALseg(startSplitIndex, endSplitIndex, slope, data)} End" << endl;
        cout << endl;
        #endif
    }

    DALseg(pair<Type_Key,Type_Ts> & singleData)
    :m_rightSearchBound(0), m_leftSearchBound(0), m_numPair(0), m_numPairExist(0), m_slope(0), m_parentIndex(0), m_maxSearchError(0)
    {
        #ifdef DEBUG
        cout << "[Debug Info:] Class {DALseg} :: Construct Function {DALseg(singleData)} Begin" << endl;
        cout << endl;
        #endif

        m_buffer.reserve(MAX_BUFFER_SIZE);

        m_buffer.push_back(singleData);
        m_numPairBuffer = 1;

        m_startKey = singleData.first;
        m_currentNodeStartKey = m_startKey;
        m_maxTimeStamp = singleData.second;

        #ifdef TUNE
        m_tuneStage = tuneStage;
        #endif

        #ifdef DEBUG
        cout << "[Debug Info:] Class {DALseg} :: Construct Function {DALseg(singleData)} End" << endl;
        cout << endl;
        #endif
    }

    void local_train(int & startSplitIndex, int & endSplitIndex, double & slope, 
                    vector<pair<Type_Key,Type_Ts>> & data);
    
    void merge_remove_gap_dp_buffer(vector<pair<Type_Key,Type_Ts>> & mergedData, Type_Ts & lowerLimit);
    
    bool lookup_seg(Type_Key & newKey, Type_Ts & lowerLimit);

    void range_search_predict(  Type_Key & newKey, Type_Ts & newTimeStamp, Type_Ts & lowerLimit,
                                Type_Key & lowerBound, Type_Key & upperBound, 
                                vector<pair<Type_Key, Type_Ts>> & rangeSearchResult,
                                vector<pair<Type_Key,int >> & updateSeg);

    void range_search(  int startSearchPos, int startSearchBufferPos, 
                        Type_Key & newKey, Type_Ts & newTimeStamp, Type_Ts & lowerLimit,
                        Type_Key & lowerBound, Type_Key & upperBound,
                        vector<pair<Type_Key, Type_Ts>> &  rangeSearchResult, 
                        vector<pair<Type_Key,int >> & updateSeg);

    void insert(Type_Key & newKey, Type_Ts & newTimeStamp, Type_Ts & lowerLimit, 
                            vector<pair<Type_Key,int >> & updateSeg);

    void insert_current_seg(Type_Key & newKey, Type_Ts & newTimeStamp, Type_Ts & lowerLimit,
                            vector<pair<Type_Key,int >> & updateSeg);

    void insert_dp( int & insertionPos, Type_Key & newKey, Type_Ts & newTimeStamp,  Type_Ts & lowerLimit, 
                    bool & gapAddError, vector<pair<Type_Key,int >> & updateSeg);

    void insert_dp_end( int & lastPos, Type_Key & newKey, Type_Ts & newTimeStamp,  Type_Ts & lowerLimit, 
                        vector<pair<Type_Key,int >> & updateSeg);

    void insert_dp_append(  int & insertionPos, Type_Key & newKey, Type_Ts & newTimeStamp,  Type_Ts & lowerLimit, 
                            vector<pair<Type_Key,int >> & updateSeg);
    
    void insert_buffer( int insertionPos, Type_Key & newKey, Type_Ts & newTimeStamp,  Type_Ts & lowerLimit,
                        vector<pair<Type_Key,int >> & updateSeg);
    

    void find_predict_pos_bound(Type_Key & targetKey, int & predictPos, int & predictPosMin, int & predictPosMax);
    void exponential_search_dp_right(Type_Key & targetKey, int & foundPos, int maxSearchBound);
    void exponential_search_dp_left(Type_Key & targetKey, int & foundPos, int maxSearchBound);
    void binary_search_lower_bound_buffer(Type_Key & targetKey, int & foundPos);
    bool index_exists_dp(int index, Type_Ts lowerLimit);
    bool index_exists_buffer(int index, Type_Ts lowerLimit);
    void print();

    friend class DALroot<Type_Key,Type_Ts>;
};

/*
Local Load and Add Gap
*/

template<class Type_Key, class Type_Ts>
void DALseg<Type_Key,Type_Ts>::local_train(int & startSplitIndex, int & endSplitIndex, double & slope, vector<pair<Type_Key,Type_Ts>> & data)
{
    #ifdef DEBUG
    cout << "[Debug Info:] Class {DALseg} :: Member Function {local_train()} Begin" << endl;
    cout << endl;
    #endif

    m_startKey = data[startSplitIndex].first;
    m_currentNodeStartKey = m_startKey;
    
    m_slope = slope * EXPANSION_RATIO;

    m_localData.push_back(data[startSplitIndex]);
    
    int currentSplitIndex = startSplitIndex + 1;
    int currentInsertionPos = 1;
    int predictedPos;

    m_maxTimeStamp = data[startSplitIndex].second;
    
    while(currentSplitIndex <= endSplitIndex)
    {
        predictedPos = static_cast<int>(floor(m_slope * ((double)data[currentSplitIndex].first - (double)m_startKey)));

        if(predictedPos == currentInsertionPos)
        {

            m_localData.push_back(data[currentSplitIndex]);

            m_maxTimeStamp = (data[currentSplitIndex].second > m_maxTimeStamp) 
                        ? data[currentSplitIndex].second 
                        : m_maxTimeStamp;

            currentSplitIndex++;
        }
        else if(predictedPos < currentInsertionPos)
        {
            m_localData.push_back(data[currentSplitIndex]);

            m_rightSearchBound = (currentInsertionPos - predictedPos) > m_rightSearchBound ? (currentInsertionPos - predictedPos) : m_rightSearchBound ;

            m_maxTimeStamp = (data[currentSplitIndex].second > m_maxTimeStamp) 
                        ? data[currentSplitIndex].second 
                        : m_maxTimeStamp;
            
            currentSplitIndex++;
        }
        else
        {
            m_localData.push_back( make_pair(m_localData.back().first,0));

        }
        currentInsertionPos++;
    }

    m_rightSearchBound += 1; //Must be greater than value itself.
    m_numPair = m_localData.size();
    m_numPairExist = (endSplitIndex - startSplitIndex + 1);
    
    #ifdef MIN_BOUND
    m_maxSearchError = min(8192,(int)ceil(SEARCH_BOUND_RATIO*m_numPair));
    #else
    m_maxSearchError = ceil(SEARCH_BOUND_RATIO*m_numPair);
    #endif

    #ifdef DEBUG
    cout << "[Debug Info:] Class {DALseg} :: Member Function {local_train()} End" << endl;
    cout << "[Debug Info:] m_rightSearchBound =  " << m_rightSearchBound << endl;
    cout << "[Debug Info:] m_maxTimeStamp =  " << m_maxTimeStamp << endl;
    cout << "[Debug Info:] m_slope =  " << m_slope << endl;
    cout << "[Debug Info:] m_numPair =  " << m_numPair << endl;
    cout << "[Debug Info:] m_numPairExist =  " << m_numPairExist << endl;
    cout << "[Debug Info:] m_localData.size() =  " << m_localData.size() << endl;
    cout << endl;
    #endif

}

/*
Combining Data and Buffer and Remove Gaps
*/

template<class Type_Key, class Type_Ts>
void DALseg<Type_Key,Type_Ts>::merge_remove_gap_dp_buffer(vector<pair<Type_Key,Type_Ts>> & mergedData, Type_Ts & lowerLimit)
{
    #ifdef TUNE_TIME
    uint64_t temp = 0;
    startTimer(&temp);
    #endif

    int localDataIndex = 0;
    int bufferIndex = 0;
    
    while(localDataIndex  < m_numPair && bufferIndex  < m_numPairBuffer)
    {
        if (m_localData[localDataIndex].first < m_buffer[bufferIndex].first)
        {
            if (m_localData[localDataIndex].second >= lowerLimit)
            {
                mergedData.push_back(m_localData[localDataIndex]);
            }
            localDataIndex++;
        }
        else if (m_localData[localDataIndex].first > m_buffer[bufferIndex].first)
        {
            if (m_buffer[bufferIndex].second >= lowerLimit)
            {
                mergedData.push_back(m_buffer[bufferIndex]);
            }
            bufferIndex++;
        }
        else
        {
            cout << "ERROR: buffer and m_localData have same value (" << m_localData[localDataIndex].first << ")" << endl;
        }
    }

    while(localDataIndex  < m_numPair)
    {
        if (m_localData[localDataIndex].second >= lowerLimit)
        {
            mergedData.push_back(m_localData[localDataIndex]);
        }
        localDataIndex++;
    }

    while (bufferIndex < m_numPairBuffer)
    {
        if (m_buffer[bufferIndex].second >= lowerLimit)
        {
            mergedData.push_back(m_buffer[bufferIndex]);
        }
        bufferIndex++;
    }

    #ifdef TUNE
    segLengthMerge += m_numPair + m_numPairBuffer;
    #endif

    #ifdef TUNE_TIME
    stopTimer(&temp);
    segLengthMerge += m_numPair + m_numPairBuffer;
    segMergeCycle += temp;
    segMergeCyclePerLength += (double)temp/(m_numPair + m_numPairBuffer);
    #endif
}

/*
Point Lookup
*/

template<class Type_Key, class Type_Ts>
bool DALseg<Type_Key,Type_Ts>::lookup_seg(Type_Key & newKey, Type_Ts & lowerLimit)
{
    #if defined DEBUG 
    cout << "[Debug Info:] Class {DALseg} :: Member Function {lookup_seg()} Begin" << endl;
    cout << endl;
    #elif defined DEBUG_KEY
    if(DEBUG_KEY == newKey)
    {
        cout << "[Debug Info:] Class {DALseg} :: Member Function {lookup_seg()} Begin" << endl;
        cout << endl;
    }
    #endif


    if (m_numPair)
    {
        int actualPos, predictPos, predictPosMin, predictPosMax;
        find_predict_pos_bound(newKey, predictPos, predictPosMin, predictPosMax);
        
        if (predictPosMin < predictPosMax)
        {
            predictPos = predictPos < predictPosMin ? predictPosMin : predictPos;
            predictPos = predictPos > predictPosMax ? predictPosMax : predictPos;
            actualPos = predictPos;

            if (newKey < m_localData[predictPos].first) // Left of predictedPos
            {  
                exponential_search_dp_left(newKey, actualPos, predictPos-predictPosMin);
            }
            else if (newKey > m_localData[predictPos].first)
            {
                exponential_search_dp_right(newKey, actualPos, predictPosMax-predictPos);
            }

        }
        else if (predictPosMin == predictPosMax)
        {
            actualPos = predictPos;
        }
        else
        {
            if (!predictPosMin)
            {
                actualPos = 0;
            }
            else
            {
                actualPos = m_numPair-1;
            }

        }

        if (m_localData[actualPos].first == newKey && m_localData[actualPos].second && m_localData[actualPos].second >= lowerLimit)
        {
            return true;
        }
    }

    if (m_numPairBuffer)
    {
        auto it = lower_bound(m_buffer.begin(),m_buffer.end(),newKey,
                    [](const pair<Type_Key,Type_Ts>& data, Type_Key value)
                    {
                        return data.first < value;
                    });
        
        if (it != m_buffer.end() && it->first == newKey && it->second && it->second >= lowerLimit)
        {
            return true;
        }
    }

    return false;

    #if defined DEBUG 
    cout << "[Debug Info:] Class {DALseg} :: Member Function {lookup_seg()} End" << endl;
    cout << endl;
    #elif defined DEBUG_KEY
    if(DEBUG_KEY == newKey)
    {
        cout << "[Debug Info:] Class {DALseg} :: Member Function {lookup_seg()} End" << endl;
        cout << endl;
    }
    #endif
}

/*
Range Search
*/

template<class Type_Key, class Type_Ts>
void DALseg<Type_Key,Type_Ts>::range_search_predict( Type_Key & newKey, Type_Ts & newTimeStamp, Type_Ts & lowerLimit, 
                                                    Type_Key & lowerBound, Type_Key & upperBound, 
                                                    vector<pair<Type_Key, Type_Ts>> & rangeSearchResult,
                                                    vector<pair<Type_Key,int >> & updateSeg)
{    
    #if defined DEBUG 
    cout << "[Debug Info:] Class {DALseg} :: Member Function {range_search_predict()} Begin" << endl;
    (searchStream)?
                    cout << "[Debug Info:] searchStream = S " << endl:
                    cout << "[Debug Info:] searchStream = R " << endl;
    cout << "[Debug Info:] newKey = " << newKey << endl;
    cout << "[Debug Info:] newTimeStamp = " << newTimeStamp << endl;
    cout << "[Debug Info:] lowerBound = " << lowerBound << endl;
    cout << "[Debug Info:] upperBound = " << upperBound << endl;
    cout << endl;
    #elif defined DEBUG_KEY
    if(DEBUG_KEY == newKey)
    {
        cout << "[Debug Info:] Class {DALseg} :: Member Function {range_search_predict()} Begin" << endl;
        cout << "[Debug Info:] newKey = " << newKey << endl;
        cout << "[Debug Info:] newTimeStamp = " << newTimeStamp << endl;
        cout << "[Debug Info:] lowerBound = " << lowerBound << endl;
        cout << "[Debug Info:] upperBound = " << upperBound << endl;        
        cout << endl;
    }
    #endif

    #if defined(TUNE_TIME) || defined(TIME_BREAKDOWN)
    #ifdef TUNE_TIME
    segNoSearch++;
    #endif
    uint64_t timer = 0;
    startTimer(&timer);
    #endif
    
    int actualPos, predictPos, predictPosMin, predictPosMax, bufferPos;
    find_predict_pos_bound(lowerBound, predictPos, predictPosMin, predictPosMax);

    binary_search_lower_bound_buffer(lowerBound,bufferPos);
    
    #if defined DEBUG 
    cout << "[Debug Info:] Class {DALseg} :: Member Function {range_search_predict()} After Predicting Pos" << endl;
    cout << "[Debug Info:] predictPosMin = " << predictPosMin << endl;
    cout << "[Debug Info:] predictPosMax = " << predictPosMax << endl;
    cout << "[Debug Info:] bufferPos = " << bufferPos << endl;
    cout << endl;
    #elif defined DEBUG_KEY
    if(DEBUG_KEY == newKey)
    {
        cout << "[Debug Info:] Class {DALseg} :: Member Function {range_search_predict()} After Predicting Pos" << endl;
        cout << "[Debug Info:] predictPosMin = " << predictPosMin << endl;
        cout << "[Debug Info:] predictPosMax = " << predictPosMax << endl;
        cout << "[Debug Info:] bufferPos = " << bufferPos << endl;
        cout << endl;
    }
    #endif

    if (predictPosMin < predictPosMax)
    {     
        predictPos = predictPos < predictPosMin ? predictPosMin : predictPos;
        predictPos = predictPos > predictPosMax ? predictPosMax : predictPos;

        if (lowerBound < m_localData[predictPos].first) // Left of predictedPos
        {
            actualPos = predictPos;
            exponential_search_dp_left(lowerBound, actualPos, predictPos-predictPosMin);
        }
        else if (lowerBound > m_localData[predictPos].first) //Right of predictedPos
        {
            actualPos = predictPos;
            exponential_search_dp_right(lowerBound, actualPos, predictPosMax-predictPos);
        }
        else // targetKey == Key at predictedPos
        {
            actualPos = predictPos;
        }     
    }
    else if (predictPosMin == predictPosMax)
    {
        actualPos = predictPos;
    }
    else
    {
        if (!predictPosMin) // Lower bound less than m_startkey
        {
            actualPos = 0;
        }
        else 
        {
            actualPos = m_numPair;
        }
    }

    #if defined(TUNE_TIME) || defined(TIME_BREAKDOWN)
    stopTimer(&timer);
    searchCycle += timer;
    #endif
    
    #ifdef TUNE
    segNoScan++;
    #endif

    #if defined(TUNE_TIME) || defined(TIME_BREAKDOWN)
    #ifdef TUNE_TIME
    segNoScan++;
    #endif
    timer = 0;
    startTimer(&timer);
    #endif

    range_search( actualPos, bufferPos, newKey, newTimeStamp, lowerLimit, lowerBound, upperBound, rangeSearchResult, updateSeg);

    #if defined(TUNE_TIME) || defined(TIME_BREAKDOWN)
    stopTimer(&timer);
    scanCycle += timer;
    #endif

    #if defined DEBUG 
    cout << "[Debug Info:] Class {DALseg} :: Member Function {range_search_predict()} End" << endl;
    cout << endl;
    #elif defined DEBUG_KEY
    if(DEBUG_KEY == newKey)
    {
        cout << "[Debug Info:] Class {DALseg} :: Member Function {range_search_predict()} End" << endl;
        cout << endl;
    }
    #endif
}

template<class Type_Key, class Type_Ts>
void DALseg<Type_Key,Type_Ts>::range_search( int startSearchPos, int startSearchBufferPos,
                                            Type_Key & newKey, Type_Ts & newTimeStamp, Type_Ts & lowerLimit, 
                                            Type_Key & lowerBound, Type_Key & upperBound, 
                                            vector<pair<Type_Key, Type_Ts>> &  rangeSearchResult,
                                            vector<pair<Type_Key,int >> & updateSeg)
{
    #if defined DEBUG 
    cout << "[Debug Info:] Class {DALseg} :: Member Function {range_search()} Begin" << endl;
    cout << "[Debug Info:] startSearchPos = " << startSearchPos << endl;
    cout << "[Debug Info:] startSearchBufferPos = " << startSearchBufferPos << endl;
    cout << endl;
    #elif defined DEBUG_KEY
    if(DEBUG_KEY == newKey)
    {
        cout << "[Debug Info:] Class {DALseg} :: Member Function {range_search()} Begin" << endl;
        cout << "[Debug Info:] m_currentNodeStartKey = " << m_currentNodeStartKey << endl;
        cout << "[Debug Info:] m_startKey = " << m_startKey << endl;
        cout << "[Debug Info:] m_maxTimeStamp = " << m_maxTimeStamp << endl;
        cout << "[Debug Info:] startSearchPos = " << startSearchPos << endl;
        cout << "[Debug Info:] startSearchBufferPos = " << startSearchBufferPos << endl;
        cout << "[Debug Info:] m_numPair = " << m_numPair << "," << m_localData.size() << endl;
        cout << "[Debug Info:] m_numPairExist = " << m_numPairExist << endl;
        cout << "[Debug Info:] m_numPairBuffer = " << m_numPairBuffer << "," << m_buffer.size() << endl;
        cout << endl;
    }
    #endif

    if (m_maxTimeStamp >= lowerLimit)
    {
        while(startSearchPos < m_numPair && m_localData[startSearchPos].first <= upperBound)
        {
            if (m_localData[startSearchPos].second && m_localData[startSearchPos].second >= lowerLimit)
            {
                rangeSearchResult.push_back(m_localData[startSearchPos]);
            }
            else
            {
                if (m_localData[startSearchPos].second)
                {
                    m_localData[startSearchPos].second = 0;
                    m_numPairExist--;
                }
            }
            startSearchPos++;
        }

        while(startSearchBufferPos < m_numPairBuffer && m_buffer[startSearchBufferPos].first <= upperBound)
        {
            if (m_buffer[startSearchBufferPos].second && m_buffer[startSearchBufferPos].second >= lowerLimit)
            {
                rangeSearchResult.push_back(m_buffer[startSearchBufferPos]);
            }
            else
            {
                if (m_buffer[startSearchBufferPos].second)
                {
                    m_buffer[startSearchBufferPos].second = 0;
                }
            }
            startSearchBufferPos++;
        }

        if ((double)m_numPairExist/m_numPair < MIN_OCCUPANCY)
        {
            updateSeg.push_back(make_pair(m_currentNodeStartKey,m_parentIndex));
        }
    }
    else //Delete Segment
    {        
        if (m_leftSibling)
        {
            m_leftSibling->m_rightSibling = m_rightSibling;
        }
        if (m_rightSibling)
        {
            m_rightSibling->m_leftSibling = m_leftSibling;
        }
        updateSeg.push_back(make_pair(numeric_limits<Type_Key>::max(),m_parentIndex));
    }


    #if defined DEBUG 
    cout << "[Debug Info:] Class {DALseg} :: Member Function {range_search()} go to sibling" << endl;
    cout << endl;
    #elif defined DEBUG_KEY
    if(DEBUG_KEY == newKey)
    {
        cout << "[Debug Info:] Class {DALseg} :: Member Function {range_search()} go to sibling" << endl;
        cout << endl;
    }
    #endif

    if(m_rightSibling && m_rightSibling->m_currentNodeStartKey <= upperBound)
    {
        m_rightSibling->range_search(0,0, newKey, newTimeStamp, lowerLimit, lowerBound, upperBound, rangeSearchResult, updateSeg);
    }

    #if defined DEBUG 
    cout << "[Debug Info:] Class {DALseg} :: Member Function {range_search()} back from sibling" << endl;
    cout << endl;
    #elif defined DEBUG_KEY
    if(DEBUG_KEY == newKey)
    {
        cout << "[Debug Info:] Class {DALseg} :: Member Function {range_search()} back from sibling" << endl;
        cout << endl;
    }
    #endif

    #ifdef TUNE
    segScanNoSeg++;
    #endif

    #ifdef TUNE_TIME
    segScanNoSeg++;
    #endif

    #if defined DEBUG 
    cout << "[Debug Info:] Class {DALseg} :: Member Function {range_search()} End" << endl;
    cout << "[Debug Info:] stop DP pos = " << startSearchPos << endl;
    cout << "[Debug Info:] stop buffer pos = " << startSearchBufferPos << endl;
    cout << endl;
    #elif defined DEBUG_KEY
    if(DEBUG_KEY == newKey)
    {
        cout << "[Debug Info:] Class {DALseg} :: Member Function {range_search()} End" << endl;
        cout << "[Debug Info:] stop DP pos = " << startSearchPos << endl;
        cout << "[Debug Info:] stop buffer pos = " << startSearchBufferPos << endl;
        cout << endl;
    }
    #endif
}

/*
Insertion
*/

template <class Type_Key, class Type_Ts>
void DALseg<Type_Key,Type_Ts>::insert(Type_Key & newKey, Type_Ts & newTimeStamp, Type_Ts & lowerLimit, 
                                    vector<pair<Type_Key,int >> & updateSeg)
{
    #if defined DEBUG 
    cout << "[Debug Info:] Class {DALseg} :: Member Function {insert()} Begin" << endl;
    cout << "[Debug Info:] newKey = "  << newKey << " , newTimeStamp = " << newTimeStamp << endl;
    cout << "[Debug Info:] m_startKey =  " << m_startKey << endl;
    cout << "[Debug Info:] m_currentNodeStartKey =  " << m_currentNodeStartKey << endl;
    cout << "[Debug Info:] m_numPair =  " << m_numPair << endl;
    cout << "[Debug Info:] m_numPairExist =  " << m_numPairExist << endl;
    cout << "[Debug Info:] m_numPairBuffer = " << m_numPairBuffer << endl; 
    cout << "[Debug Info:] m_maxTimeStamp =  " << m_maxTimeStamp << endl;
    cout << "[Debug Info:] m_slope =  " << m_slope << endl;
    cout << "[Debug Info:] m_rightSearchBound =  " << m_rightSearchBound << endl;
    cout << "[Debug Info:] m_leftSearchBound =  " << m_leftSearchBound << endl;
    cout << endl;
    #elif defined DEBUG_KEY
    if(DEBUG_KEY == newKey)
    {
        cout << "[Debug Info:] Class {DALseg} :: Member Function {insert()} Begin" << endl;
        cout << "[Debug Info:] newKey = "  << newKey << " , newTimeStamp = " << newTimeStamp << endl;
        cout << "[Debug Info:] m_startKey =  " << m_startKey << endl;
        cout << "[Debug Info:] m_currentNodeStartKey =  " << m_currentNodeStartKey << endl;
        cout << "[Debug Info:] m_numPair =  " << m_numPair << endl;
        cout << "[Debug Info:] m_numPairExist =  " << m_numPairExist << endl;
        cout << "[Debug Info:] m_numPairBuffer = " << m_numPairBuffer << endl; 
        cout << "[Debug Info:] m_maxTimeStamp =  " << m_maxTimeStamp << endl;
        cout << "[Debug Info:] m_slope =  " << m_slope << endl;
        cout << "[Debug Info:] m_rightSearchBound =  " << m_rightSearchBound << endl;
        cout << "[Debug Info:] m_leftSearchBound =  " << m_leftSearchBound << endl;
        cout << endl;
    }
    #endif

    if (m_maxTimeStamp >= lowerLimit)
    {
        insert_current_seg(newKey, newTimeStamp, lowerLimit, updateSeg);

        if (!updateSeg.size() && (double)m_numPairExist/m_numPair < MIN_OCCUPANCY)
        {
            updateSeg.push_back(make_pair(m_currentNodeStartKey, m_parentIndex*10+1));
        }
    }
    else
    {       
        #ifdef TUNE_TIME
        segNoInsert++;
        uint64_t temp = 0;
        startTimer(&temp);
        #endif

        //Segment expired (need to delete segement)
        updateSeg.push_back(make_pair(numeric_limits<Type_Key>::max(),m_parentIndex));

        if(m_leftSibling) 
        {
            #if defined DEBUG 
            cout << "[Debug Info:] Class {DALseg} :: Member Function {insert()} Insert into left neighbour" << endl;
            cout << endl;
            #elif defined DEBUG_KEY
            if(DEBUG_KEY == newKey)
            {
                cout << "[Debug Info:] Class {DALseg} :: Member Function {insert()} Insert into left neighbour" << endl;
                cout << endl;
            }
            #endif
            
            m_leftSibling->m_buffer.push_back(make_pair(newKey,newTimeStamp));

            m_leftSibling->m_numPairBuffer++;

            m_leftSibling->m_rightSibling = m_rightSibling;

            if (m_rightSibling)
            {
                m_rightSibling->m_leftSibling = m_leftSibling;
            }

            m_leftSibling->m_maxTimeStamp = m_leftSibling->m_maxTimeStamp < newTimeStamp ? newTimeStamp : m_leftSibling->m_maxTimeStamp;

            if (m_leftSibling->m_numPairBuffer >= MAX_BUFFER_SIZE-1)
            {
                updateSeg.push_back(make_pair(m_leftSibling->m_currentNodeStartKey, m_leftSibling->m_parentIndex*10+1));
                
            }
        }
        else if(m_rightSibling)
        {   
            #if defined DEBUG 
            cout << "[Debug Info:] Class {DALseg} :: Member Function {insert()} Insert into right neighbour" << endl;
            cout << endl;
            #elif defined DEBUG_KEY
            if(DEBUG_KEY == newKey)
            {
                cout << "[Debug Info:] Class {DALseg} :: Member Function {insert()} Insert into right neighbour" << endl;
                cout << endl;
            }
            #endif

            m_rightSibling->insert_buffer(0, newKey, newTimeStamp, lowerLimit, updateSeg);

            m_rightSibling->m_leftSibling = m_leftSibling;
            m_rightSibling->m_currentNodeStartKey = newKey;

            m_rightSibling->m_maxTimeStamp = m_rightSibling->m_maxTimeStamp < newTimeStamp ? newTimeStamp : m_rightSibling->m_maxTimeStamp;

            if (updateSeg.size() == 1) //If retrain == null
            {
                updateSeg.push_back(make_pair(m_rightSibling->m_currentNodeStartKey, m_rightSibling->m_parentIndex*10));
            }
        }
        else
        {
            cout << "Invalid Insertion" << endl;
        }

        #ifdef TUNE_TIME
        stopTimer(&temp);
        segInsertCycle += temp;
        #endif
    }

    #if defined DEBUG 
    cout << "[Debug Info:] Class {DALseg} :: Member Function {insert()} End" << endl;

    cout << "[Debug Info:] Printing m_localData after insertion : " << endl;
    cout << "[Debug Info:] m_numPair =  " << m_numPair << endl;
    cout << "[Debug Info:] m_numPairExist =  " << m_numPairExist << endl;
    cout << "[Debug Info:] m_numPairBuffer = " << m_numPairBuffer << endl; 
    cout << "[Debug Info:] m_maxTimeStamp =  " << m_maxTimeStamp << endl;
    cout << "[Debug Info:] m_rightSearchBound =  " << m_rightSearchBound << endl;
    cout << "[Debug Info:] m_leftSearchBound =  " << m_leftSearchBound << endl;
    cout << endl;
    #elif defined DEBUG_KEY
    if(DEBUG_KEY == newKey)
    {
        cout << "[Debug Info:] Class {DALseg} :: Member Function {insert()} End" << endl;
        cout << "[Debug Info:] m_numPair =  " << m_numPair << endl;
        cout << "[Debug Info:] m_numPairExist =  " << m_numPairExist << endl;
        cout << "[Debug Info:] m_numPairBuffer = " << m_numPairBuffer << endl; 
        cout << "[Debug Info:] m_maxTimeStamp =  " << m_maxTimeStamp << endl;
        cout << "[Debug Info:] m_rightSearchBound =  " << m_rightSearchBound << endl;
        cout << "[Debug Info:] m_leftSearchBound =  " << m_leftSearchBound << endl;
        cout << endl;
    }
    #endif
}


template <class Type_Key, class Type_Ts>
void DALseg<Type_Key,Type_Ts>::insert_current_seg(   Type_Key & newKey, Type_Ts & newTimeStamp, Type_Ts & lowerLimit, 
                                                    vector<pair<Type_Key,int >> & updateSeg)
{
    #if defined(TUNE_TIME) || defined(TIME_BREAKDOWN)
    #ifdef TUNE_TIME
    segNoInsert++;
    #endif
    uint64_t temp = 0;
    uint64_t temp2 = 0;
    startTimer(&temp);
    #endif

    if (newKey < m_startKey || !m_numPair)
    {
        #if defined DEBUG 
        cout << "[Debug Info:] newKey < m_startKey or empty DP, insert into buffer" << endl;
        cout << endl;
        #elif defined DEBUG_KEY
        if(DEBUG_KEY == newKey)
        {
            cout << "[Debug Info:] newKey < m_startKey or empty DP, insert into buffer" << endl;
            cout << endl;
        }
        #endif
        
        insert_buffer(-1, newKey, newTimeStamp, lowerLimit, updateSeg);
    }
    else
    {
        #if defined(TUNE_TIME) || defined(TIME_BREAKDOWN)
        startTimer(&temp2);
        uint64_t temp3 = 0;
        startTimer(&temp3);
        #ifdef TUNE_TIME
        segNoSearch++;
        #endif
        #endif

        int insertionPos, predictPos, predictPosMin, predictPosMax;

        find_predict_pos_bound(newKey, predictPos, predictPosMin, predictPosMax);

        #if defined(TUNE_TIME) || defined(TIME_BREAKDOWN)
        stopTimer(&temp3);
        #endif

        //Insertion if predicted pos bound is within DP
        if (predictPosMin < predictPosMax)
        {
            predictPos = predictPos < predictPosMin ? predictPosMin : predictPos;
            predictPos = predictPos > predictPosMax ? predictPosMax : predictPos;

            if (newKey < m_localData[predictPos].first) // Left of predictedPos
            {
                insertionPos = predictPos;
                exponential_search_dp_left(newKey, insertionPos, predictPos-predictPosMin);
            }
            else if (newKey > m_localData[predictPos].first) //Right of predictedPos
            {
                insertionPos = predictPos;
                exponential_search_dp_right(newKey, insertionPos, predictPosMax-predictPos);
            }
            else // newKey == Key at predictedPos (Will not trigger, but nesscary to keep this else)
            {
                insertionPos = predictPos;
            }

            #if defined(TUNE_TIME) || defined(TIME_BREAKDOWN)
            stopTimer(&temp2);
            searchCycle += temp2 - temp3;
            #endif

            bool gapAddError = false;
            if (insertionPos > predictPosMax)
            {
                gapAddError = true;
            }

            //Insert if insertion pos is less than last index
            if (insertionPos < m_numPair)
            {                
                insert_dp(insertionPos, newKey, newTimeStamp, lowerLimit, gapAddError, updateSeg);
            }
            //Else append
            else
            {
                //Append and increment m_rightSearchBound
                m_localData.push_back(make_pair(newKey,newTimeStamp));
                m_numPair++;
                m_numPairExist++;
                m_rightSearchBound++;
            }

        }
        //Check last position to append or insert
        else if (predictPosMin == predictPosMax)
        {   
            #if defined(TUNE_TIME) || defined(TIME_BREAKDOWN)
            stopTimer(&temp2);
            searchCycle += temp2 - temp3;
            #endif

            insert_dp_end(predictPosMin, newKey, newTimeStamp, lowerLimit, updateSeg);

        }
        //Insertion is append type 
        else
        {
            #if defined(TUNE_TIME) || defined(TIME_BREAKDOWN)
            stopTimer(&temp2);
            searchCycle += temp2 - temp3;
            #endif

            insertionPos = predictPosMin + m_leftSearchBound;

            insert_dp_append(insertionPos, newKey, newTimeStamp, lowerLimit, updateSeg);
        }
        
        #ifdef MIN_BOUND
        m_maxSearchError = min(8192,(int)ceil(SEARCH_BOUND_RATIO*m_numPair));
        #else
        m_maxSearchError = ceil(SEARCH_BOUND_RATIO*m_numPair);
        #endif
    }

    m_maxTimeStamp = (m_maxTimeStamp < newTimeStamp)? newTimeStamp: m_maxTimeStamp;

    #if defined(TUNE_TIME) || defined(TIME_BREAKDOWN)
    stopTimer(&temp);
    insertCycle += temp-temp2;
    #ifdef TUNE_TIME
    segInsertCycle += temp-temp2;
    #endif
    #endif
}


template <class Type_Key, class Type_Ts>
void DALseg<Type_Key,Type_Ts>::insert_dp(int & insertionPos, Type_Key & newKey, Type_Ts & newTimeStamp, Type_Ts & lowerLimit,
                                        bool & gapAddError, vector<pair<Type_Key,int >> & updateSeg)
{
    //If insertionPos is not a gap
    if (index_exists_dp(insertionPos, lowerLimit))
    {
        if (m_leftSearchBound + m_rightSearchBound >= m_maxSearchError)
        {
            insert_buffer(-1, newKey, newTimeStamp, lowerLimit, updateSeg);
        }
        else
        {
            /*
            Find shifting position
                First find right gap (shiftOrNot = true)
                    It might reach the end and find no gap. (That is fine, record the distance to the end) (shiftOrNot = false)
                Then find left gap that is closer to the insertionPos (shiftOrNot = true)

            If no gap within ShiftCost (left and right)
                Insert to Buffer

            If (shiftOrNot = true)
                "Move" keys using the move() function and insert into insertion position
                    increment m_rightSearchBound or m_leftSearchBound depending on the location of the gap.
            Else
                "Insert" keys using the v.insert() function (no gaps, and closer to the end of the DP)
                    increment m_rightSearchBound
            */

            int gapPos = -1;
            bool shiftOrNot = false;
            int rightGapPos, leftGapPos;

            //Find Left Gap
            int minDiffGap = (insertionPos-MAX_NO_SHIFT < 0)? 0 : insertionPos-MAX_NO_SHIFT;
            for (leftGapPos = insertionPos; leftGapPos >= minDiffGap; leftGapPos--)
            {
                if (!index_exists_dp(leftGapPos, lowerLimit))
                {
                    gapPos = leftGapPos;
                    shiftOrNot = true;
                    break;
                }
            }

            //Find Right Gap
            minDiffGap = (gapPos == -1) ? insertionPos + MAX_NO_SHIFT : insertionPos + (insertionPos - gapPos);
            minDiffGap = (minDiffGap > m_numPair) ? m_numPair : minDiffGap;
            for (rightGapPos = insertionPos; rightGapPos < minDiffGap; rightGapPos++)
            {
                if (!index_exists_dp(rightGapPos, lowerLimit))
                {
                    gapPos = rightGapPos;
                    shiftOrNot = true;
                    break;

                }
            }

            //No Gap and reached the end (end is still within MAX_NO_SHIFT)
            if (rightGapPos == m_numPair)
            {
                gapPos = m_numPair;
                shiftOrNot = false;
            }
            
            //Insert After finding Gap
            if (gapPos == -1)
            {   
                #if defined DEBUG 
                cout << "[Debug Info:] No gaps within shift cost, insert to buffer" << endl;
                cout << endl;
                #elif defined DEBUG_KEY
                if(DEBUG_KEY == newKey)
                {
                    cout << "[Debug Info:] No gaps within shift cost, insert to buffer" << endl;
                    cout << endl;
                }
                #endif

                insert_buffer(-1, newKey, newTimeStamp, lowerLimit, updateSeg);
                
            }
            //Gap exist within shiftcost
            else
            {
                if (shiftOrNot)
                {
                    if (insertionPos < gapPos)
                    {
                        #if defined DEBUG 
                        cout << "[Debug Info:] Shift towards a gap right of the insertionPos" << endl;
                        cout << "[Debug Info:] InsertionPos = " << insertionPos << endl;
                        cout << "[Debug Info:] key at InsertionPos = " << m_localData[insertionPos].first << endl;
                        cout << "[Debug Info:] Shift Bound = (" << insertionPos << "," << gapPos << ")" << endl;
                        cout << endl;
                        #elif defined DEBUG_KEY
                        if(DEBUG_KEY == newKey)
                        {
                            cout << "[Debug Info:] Shift towards a gap right of the insertionPos" << endl;
                            cout << "[Debug Info:] InsertionPos = " << insertionPos << endl;
                            cout << "[Debug Info:] key at InsertionPos = " << m_localData[insertionPos].first << endl;
                            cout << "[Debug Info:] Shift Bound = (" << insertionPos << "," << gapPos << ")" << endl;
                            cout << endl;
                        }
                        #endif

                        vector<pair<Type_Key,Type_Ts>> tempMove(m_localData.begin()+insertionPos,m_localData.begin()+gapPos);
                        move(tempMove.begin(),tempMove.end(),m_localData.begin()+insertionPos+1);
                        
                        m_localData[insertionPos] = make_pair(newKey,newTimeStamp);
                        m_numPairExist++;
                        m_rightSearchBound++;

                    }
                    //Gap on the left -> move from insertion pos -1 and insert at insertion pos - 1
                    else 
                    {
                        #if defined DEBUG 
                        cout << "[Debug Info:] Shift towards a gap left of the insertionPos" << endl;
                        cout << "[Debug Info:] InsertionPos = " << insertionPos-1  << endl;
                        cout << "[Debug Info:] key at InsertionPos = " << m_localData[insertionPos-1 ].first << endl;
                        cout << "[Debug Info:] Shift Bound = (" << gapPos << "," << insertionPos-1 << ")" << endl;
                        cout << endl;
                        #elif defined DEBUG_KEY
                        if(DEBUG_KEY == newKey)
                        {
                            cout << "[Debug Info:] Shift towards a gap left of the insertionPos" << endl;
                            cout << "[Debug Info:] InsertionPos = " << insertionPos-1  << endl;
                            cout << "[Debug Info:] key at InsertionPos = " << m_localData[insertionPos-1 ].first << endl;
                            cout << "[Debug Info:] Shift Bound = (" << gapPos << "," << insertionPos-1 << ")" << endl;
                            cout << endl;
                        }
                        #endif

                        insertionPos--;
                        vector<pair<Type_Key,Type_Ts>> tempMove(m_localData.begin()+gapPos+1,m_localData.begin()+insertionPos+1);
                        move(tempMove.begin(),tempMove.end(),m_localData.begin()+gapPos);

                        m_localData[insertionPos] = make_pair(newKey,newTimeStamp);
                        m_numPairExist++;
                        m_leftSearchBound++;
                    }                            
                }
                else
                {
                    #if defined DEBUG 
                    cout << "[Debug Info:] Minimium distance is to the end of the DP, there insert at insertion positon" << endl;
                    cout << "[Debug Info:] InsertionPos = " << insertionPos  << endl;
                    cout << "[Debug Info:] key at InsertionPos = " << m_localData[insertionPos].first << endl;
                    cout << endl;
                    #elif defined DEBUG_KEY
                    if(DEBUG_KEY == newKey)
                    {
                        cout << "[Debug Info:] Minimium distance is to the end of the DP, there insert at insertion positon" << endl;
                        cout << "[Debug Info:] InsertionPos = " << insertionPos  << endl;
                        cout << "[Debug Info:] key at InsertionPos = " << m_localData[insertionPos].first << endl;
                        cout << endl;
                    }
                    #endif
                    
                    //Insert without gaps
                    m_localData.insert(m_localData.begin()+insertionPos,make_pair(newKey,newTimeStamp));
                    m_numPair++;
                    m_numPairExist++;
                    m_rightSearchBound++;
                }
            }
        }
        
    }
    //If insertion pos is a gap
    else
    {
        if (gapAddError && m_leftSearchBound + m_rightSearchBound >= m_maxSearchError)
        {
            insert_buffer(-1, newKey, newTimeStamp, lowerLimit, updateSeg);
        }
        else
        {
            #if defined DEBUG 
            cout << "[Debug Info:] Insertion Position is a Gap, change all empty keys after insertionPos to have key = newKey" << endl;
            cout << "[Debug Info:] InsertionPos = " << insertionPos  << endl;
            cout << "[Debug Info:] key at InsertionPos = " << m_localData[insertionPos].first << endl;
            cout << endl;
            #elif defined DEBUG_KEY
            if(DEBUG_KEY == newKey)
            {
                cout << "[Debug Info:] Insertion Position is a Gap, change all empty keys after insertionPos to have key = newKey" << endl;
                cout << "[Debug Info:] InsertionPos = " << insertionPos  << endl;
                cout << "[Debug Info:] key at InsertionPos = " << m_localData[insertionPos].first << endl;
                cout << endl;
            }
            #endif
            
            m_localData[insertionPos].first = newKey;
            m_localData[insertionPos].second = newTimeStamp;
            m_numPairExist++;
            int nextPos = insertionPos + 1;
            while (nextPos < m_numPair && m_localData[nextPos].first < newKey)
            {
                m_localData[nextPos].first = newKey;
                nextPos++;
            }
            
            if (gapAddError)
            {
                m_rightSearchBound++;
            }
        }
    }
}


template <class Type_Key, class Type_Ts>
void DALseg<Type_Key,Type_Ts>::insert_dp_end(int & lastPos, Type_Key & newKey, Type_Ts & newTimeStamp, Type_Ts & lowerLimit, 
                                            vector<pair<Type_Key,int >> & updateSeg)
{
    //If last key in not a gap
    if (index_exists_dp(lastPos, lowerLimit))
    {
        if (newKey < m_localData[lastPos].first)
        {
            #if defined DEBUG 
            cout << "[Debug Info:] newKey is smaller than last key, insert into last key position" << endl;
            cout << "[Debug Info:] InsertionPos = " << lastPos  << endl;
            cout << "[Debug Info:] key at InsertionPos = " << m_localData[lastPos].first << endl;
            cout << endl;
            #elif defined DEBUG_KEY
            if(DEBUG_KEY == newKey)
            {
                cout << "[Debug Info:] newKey is smaller than last key, insert into last key position" << endl;
                cout << "[Debug Info:] InsertionPos = " << lastPos  << endl;
                cout << "[Debug Info:] key at InsertionPos = " << m_localData[lastPos].first << endl;
                cout << endl;
            }
            #endif

            if (m_leftSearchBound + m_rightSearchBound >= m_maxSearchError)
            {
                insert_buffer(-1, newKey, newTimeStamp, lowerLimit, updateSeg);
            }
            else
            {
                //Insert and increment m_rightSearchBound
                m_localData.insert(m_localData.end()-1,make_pair(newKey,newTimeStamp));
                m_numPair++;
                m_numPairExist++;
                m_rightSearchBound++;
            }

        }
        else //newKey > m_localData[lastPos].first 
        {
            #if defined DEBUG 
            cout << "[Debug Info:] newKey is larger than last key, append after the last position" << endl;
            cout << "[Debug Info:] InsertionPos = " << lastPos  << endl;
            cout << "[Debug Info:] key at InsertionPos = " << m_localData[lastPos].first << endl;
            cout << endl;
            #elif defined DEBUG_KEY
            if(DEBUG_KEY == newKey)
            {
                cout << "[Debug Info:] newKey is larger than last key, append after the last position" << endl;
                cout << "[Debug Info:] InsertionPos = " << lastPos  << endl;
                cout << "[Debug Info:] key at InsertionPos = " << m_localData[lastPos].first << endl;
                cout << endl;
            }
            #endif

            //Append and increment m_rightSearchBound
            m_localData.push_back(make_pair(newKey,newTimeStamp));
            m_numPair++;
            m_numPairExist++;
        }
    }
    //Last key is a gap
    else
    {
        #if defined DEBUG 
        cout << "[Debug Info:] Last key is a gap" << endl;
        cout << "[Debug Info:] InsertionPos = " << lastPos  << endl;
        cout << "[Debug Info:] key at InsertionPos = " << m_localData[lastPos].first << endl;
        cout << endl;
        #elif defined DEBUG_KEY
        if(DEBUG_KEY == newKey)
        {
            cout << "[Debug Info:] Last key is a gap" << endl;
            cout << "[Debug Info:] InsertionPos = " << lastPos  << endl;
            cout << "[Debug Info:] key at InsertionPos = " << m_localData[lastPos].first << endl;
            cout << endl;
        }
        #endif

        m_localData[lastPos].first = newKey;
        m_localData[lastPos].second = newTimeStamp;
        m_numPairExist++;
    }
}

template <class Type_Key, class Type_Ts>
void DALseg<Type_Key,Type_Ts>::insert_dp_append( int & insertionPos, Type_Key & newKey, Type_Ts & newTimeStamp, Type_Ts & lowerLimit,
                                                vector<pair<Type_Key,int >> & updateSeg)
{
    //If gaps makes the segment very empty.
    if (((double)m_numPairExist+1) / (insertionPos+1) < MIN_OCCUPANCY)
    {
        #if defined DEBUG 
        cout << "[Debug Info:] Append type insertion - number of gaps exceed threshold - insert into buffer" << endl;
        cout << "[Debug Info:] InsertionPos = " << insertionPos  << endl;
        cout << endl;
        #elif defined DEBUG_KEY
        if(DEBUG_KEY == newKey)
        {
            cout << "[Debug Info:] Append type insertion - number of gaps exceed threshold - insert into buffer" << endl;
            cout << "[Debug Info:] InsertionPos = " << insertionPos  << endl;
            cout << endl;
        }
        #endif

        //Insert into buffer
        insert_buffer(-1, newKey, newTimeStamp, lowerLimit, updateSeg);
    }
    else
    {
        #if defined DEBUG 
        cout << "[Debug Info:] Append type insertion - number of gaps  does exceed threshold - append to dp" << endl;
        cout << "[Debug Info:] InsertionPos = " << insertionPos  << endl;
        cout << endl;
        #elif defined DEBUG_KEY
        if(DEBUG_KEY == newKey)
        {
            cout << "[Debug Info:] Append type insertion - number of gaps  does exceed threshold - append to dp" << endl;
            cout << "[Debug Info:] InsertionPos = " << insertionPos  << endl;
            cout << endl;
        }
        #endif

        //Append and copy last key to every position before insertion pos
        pair<Type_Key,Type_Ts> lastKeyForGaps = make_pair(m_localData.back().first,0);

        for (int index = m_numPair; index <= insertionPos; index++)
        {
            m_localData.push_back(lastKeyForGaps);
        }
        m_localData.back().first = newKey;
        m_localData.back().second = newTimeStamp;

        m_numPair = m_localData.size();
        m_numPairExist++;
    }
}

template <class Type_Key, class Type_Ts>
void DALseg<Type_Key,Type_Ts>::insert_buffer(int insertionPos, Type_Key & newKey, Type_Ts & newTimeStamp, Type_Ts & lowerLimit,
                                            vector<pair<Type_Key,int >> & updateSeg)
{
    
    if (insertionPos == -1)
    {
        binary_search_lower_bound_buffer(newKey, insertionPos);
    }
    
    //If buffer is not empty
    if (m_numPairBuffer && insertionPos != m_numPairBuffer)
    {   
        if (index_exists_buffer(insertionPos, lowerLimit))
        {
            //Insert type insertion
            #if defined DEBUG 
            cout << "[Debug Info:] Buffer insertion - insert" << endl;
            cout << endl;
            #elif defined DEBUG_KEY
            if(DEBUG_KEY == newKey)
            {
                cout << "[Debug Info:] Buffer insertion - insert" << endl;
                cout << endl;
            }
            #endif
            
            //If previous position is empty, then insert to previous position
            if (insertionPos && !index_exists_buffer(insertionPos-1, lowerLimit))
            {
                //Directly replace keys with insertion key
                m_buffer[insertionPos-1].first = newKey;
                m_buffer[insertionPos-1].second = newTimeStamp;

            }
            else //Shift Buffer
            {
                m_buffer.insert(m_buffer.begin()+insertionPos, make_pair(newKey,newTimeStamp));
                m_numPairBuffer++;
            }
        }
        //Gap in buffer
        else
        {
            #if defined DEBUG 
            cout << "[Debug Info:] Buffer insertion - insertionPos is a gap" << endl;
            cout << "[Debug Info:] InsertionPos = " << insertionPos  << endl;
            // cout << "[Debug Info:] key at bufferPos = " << m_buffer[insertionPos].first << endl;
            cout << endl;
            #elif defined DEBUG_KEY
            if(DEBUG_KEY == newKey)
            {
                cout << "[Debug Info:] Buffer insertion - insertionPos is a gap" << endl;
                cout << "[Debug Info:] InsertionPos = " << insertionPos  << endl;
                // cout << "[Debug Info:] key at bufferPos = " << m_buffer[insertionPos].first << endl;
                cout << endl;
            }
            #endif
            
            //Directly replace keys with insertion key
            m_buffer[insertionPos].first = newKey;
            m_buffer[insertionPos].second = newTimeStamp;
        }
    }
    else
    {
        #if defined DEBUG 
        cout << "[Debug Info:] Buffer empty or insertion pos is end, push back" << endl;
        cout << endl;
        #elif defined DEBUG_KEY
        if(DEBUG_KEY == newKey)
        {
            cout << "[Debug Info:] Buffer empty or insertion pos is end, push back" << endl;
            cout << endl;
        }
        #endif
          
        m_buffer.push_back(make_pair(newKey,newTimeStamp));
        m_numPairBuffer++;
    }

    if (m_numPairBuffer >= MAX_BUFFER_SIZE-1)
    {
        updateSeg.push_back(make_pair(m_currentNodeStartKey, m_parentIndex*10+1));

        #ifdef COUT_AUTOTUNE_SEARCH_ERROR
        noDPRetrainBufferMax++;
        #endif
    }
}

/*
Util Functions
*/

template<class Type_Key, class Type_Ts>
inline void DALseg<Type_Key,Type_Ts>::find_predict_pos_bound(Type_Key & targetKey, int & predictPos, int & predictPosMin, int & predictPosMax)
{
    #ifdef TUNE_TIME
    segNoPredict++;
    uint64_t temp = 0;
    startTimer(&temp);
    #endif

    predictPos = static_cast<int>(floor(m_slope * ((double)targetKey - (double)m_startKey)));
    predictPosMin = predictPos - m_leftSearchBound;
    predictPosMin = predictPosMin < 0 ? 0 : predictPosMin;

    predictPosMax = (m_rightSearchBound == 0) ? predictPos + 1 : predictPos + m_rightSearchBound;
    predictPosMax = predictPosMax > m_numPair-1 ? m_numPair-1: predictPosMax;

    #ifdef TUNE_TIME
    stopTimer(&temp);
    segPredictCycle += temp;
    #endif 
}

template<class Type_Key, class Type_Ts>
inline void DALseg<Type_Key,Type_Ts>::exponential_search_dp_right(Type_Key & targetKey, int & foundPos, int maxSearchBound)
{
    #ifdef TUNE
    int predictPos = foundPos;
    #endif

    #ifdef TUNE_TIME
    int predictPos = foundPos;
    uint64_t temp = 0;
    startTimer(&temp);
    #endif

    int index = 1;
    while (index <= maxSearchBound && m_localData[foundPos + index].first < targetKey)
    {
        index *= 2;
    }
    
    auto startIt = m_localData.begin() + (foundPos + static_cast<int>(index/2));
    auto endIt = m_localData.begin() + (foundPos + min(index,maxSearchBound));

    auto lowerBoundIt = lower_bound(startIt,endIt,targetKey,
                        [](const pair<Type_Key,Type_Ts>& data, Type_Key value)
                        {
                            return data.first < value;
                        });
    
    if (lowerBoundIt->first < targetKey && lowerBoundIt->second)
    {
        lowerBoundIt++;
    }

    foundPos = lowerBoundIt - m_localData.begin();

    #ifdef TUNE
    if (m_tuneStage == tuneStage)
    {
        segNoExponentialSearch++;
        segLengthExponentialSearch += foundPos - predictPos;
    }
    segNoExponentialSearchAll++;
    segLengthExponentialSearchAll += foundPos - predictPos;
    #endif

    #ifdef TUNE_TIME
    stopTimer(&temp);
    segNoExponentialSearchAll++;
    segLengthExponentialSearchAll += foundPos - predictPos;
    segExponentialSearchCycle += temp;
    if (foundPos == predictPos)
    {
        segExponentialSearchCyclePerLength += (double)temp;
    }
    else
    {
        segExponentialSearchCyclePerLength += (double)temp/(foundPos - predictPos);
    }
    #endif
}

template<class Type_Key, class Type_Ts>
inline void DALseg<Type_Key,Type_Ts>::exponential_search_dp_left(Type_Key & targetKey, int & foundPos, int maxSearchBound)
{
    #ifdef TUNE
    int predictPos = foundPos;
    #endif

    #ifdef TUNE_TIME
    int predictPos = foundPos;
    uint64_t temp = 0;
    startTimer(&temp);
    #endif

    int index = 1;
    while (index <= maxSearchBound && m_localData[foundPos - index].first >= targetKey)
    {
        index *= 2;
    }
    
    auto startIt = m_localData.begin() + (foundPos - min(index,maxSearchBound));
    auto endIt = m_localData.begin() + (foundPos - static_cast<int>(index/2));

    auto lowerBoundIt = lower_bound(startIt,endIt,targetKey,
                        [](const pair<Type_Key,Type_Ts>& data, Type_Key value)
                        {
                            return data.first < value;
                        });
    
    if (lowerBoundIt->first < targetKey && lowerBoundIt->second)
    {
        lowerBoundIt++;
    }

    foundPos = lowerBoundIt - m_localData.begin();

    #ifdef TUNE
    if (m_tuneStage == tuneStage)
    {
        segNoExponentialSearch++;
        segLengthExponentialSearch += predictPos-foundPos;
    }
    segNoExponentialSearchAll++;
    segLengthExponentialSearchAll += predictPos-foundPos;
    #endif

    #ifdef TUNE_TIME
    stopTimer(&temp);
    segNoExponentialSearchAll++;
    segLengthExponentialSearchAll += predictPos-foundPos;
    segExponentialSearchCycle += temp;
    if (foundPos == predictPos)
    {
        segExponentialSearchCyclePerLength += (double)temp;
    }
    else
    {
        segExponentialSearchCyclePerLength += (double)temp/(predictPos-foundPos);
    }
    #endif
}


template<class Type_Key, class Type_Ts>
inline void DALseg<Type_Key,Type_Ts>::binary_search_lower_bound_buffer(Type_Key & targetKey, int & foundPos)
{
    #ifdef TUNE_TIME
    bufferNoSearch++;   
    bufferLengthBinarySearch += m_numPairBuffer;
    uint64_t temp = 0;
    startTimer(&temp);
    #endif

    auto lowerBoundIt = lower_bound(m_buffer.begin(),m_buffer.end(),targetKey,
                        [](const pair<Type_Key,Type_Ts>& data, Type_Key value)
                        {
                            return data.first < value;
                        });

    foundPos = lowerBoundIt - m_buffer.begin();

    #ifdef TUNE_TIME
    stopTimer(&temp);
    bufferBinarySearchCycle += temp;
    if (m_numPairBuffer)
    {
        bufferBinarySearchCyclePerLength += (double)temp/ m_numPairBuffer;
    }
    #endif
}

template <class Type_Key, class Type_Ts>
inline bool DALseg<Type_Key,Type_Ts>::index_exists_dp(int index, Type_Ts lowerLimit)
{   
    if(m_localData[index].second)
    {
        if (m_localData[index].second < lowerLimit)
        {
            m_localData[index].second = 0 ;
            m_numPairExist--;
            return false;
        }
        return true;
    }
    return false;
}

template <class Type_Key, class Type_Ts>
inline bool DALseg<Type_Key,Type_Ts>::index_exists_buffer(int index, Type_Ts lowerLimit)
{   
    if(m_buffer[index].second)
    {
        if(m_buffer[index].second < lowerLimit)
        {
            m_buffer[index].second = 0;
            return false;
        }
        return true;
    }
    return false;
}

template <class Type_Key, class Type_Ts>
inline void DALseg<Type_Key,Type_Ts>::print()
{
    cout << "start key:" << m_currentNodeStartKey << ", slope:" << m_slope << endl;
    if (m_numPair)
    {
        cout << "data:" << endl;
        cout << m_localData[0].first << "(" << m_localData[0].second << ")";
        for (int i = 1; i < m_numPair; i++)
        {
            cout << ",";
            cout << m_localData[i].first << "(" << m_localData[i].second << ")";
        }
        cout << endl;
    }

    if (m_numPairBuffer)
    {
        cout << "buffer:" << endl;
        cout << m_buffer[0].first << "(" << m_buffer[0].second << ")";
        for (int i = 1; i < m_numPairBuffer; i++)
        {
            cout << ",";
            cout << m_buffer[i].first << "(" << m_buffer[i].second << ")";
        }
        cout << endl;
    }
}