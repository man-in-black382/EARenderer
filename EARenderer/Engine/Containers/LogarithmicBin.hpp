//
//  LogarithmicBin.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 1/4/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef LogarithmicBin_hpp
#define LogarithmicBin_hpp

#include <unordered_set>
#include <unordered_map>
#include <random>

#include "StringUtils.hpp"

namespace EARenderer {
    
    // Based on paper http://www.cg.tuwien.ac.at/research/publications/2009/cline-09-poisson/cline-09-poisson-paper.pdf
    
    template <class T>
    class LogarithmicBin {
    private:
        
#pragma mark - Nested types
        
        using Index = uint64_t;
        
        struct BinObject {
            T object;
            float weight = 0.0f;
            
            BinObject(const T& object, float weight) : object(object), weight(weight) { }
        };
    	
        struct Bin {
            std::unordered_map<Index, BinObject> objects;
            float totalWeight = 0.0f;
        };
        
        using MapIterator = typename std::unordered_map<Index, Bin>::iterator;
        using NestedMapIterator = typename std::unordered_map<Index, BinObject>::iterator;
        
#pragma mark Forward iterator
        
    public:
        class ForwardIterator {
        private:
            friend LogarithmicBin;
            
            MapIterator mMapIterator;
            MapIterator mMapEndIterator;
            NestedMapIterator mNestedMapIterator;
            
            ForwardIterator(MapIterator i, MapIterator endIterator, NestedMapIterator binIterator)
            :
            mMapIterator(i),
            mMapEndIterator(endIterator),
            mNestedMapIterator(binIterator)
            { }
            
        public:
            ForwardIterator& operator++() {
                if (mMapIterator == mMapEndIterator) {
                    throw std::out_of_range("Incrementing an iterator which had reached the end already");
                }
                
                if (mNestedMapIterator != mMapIterator->second.end()) {
                    // Increment iterator of current vector if it had not reached its end already
                    mNestedMapIterator++;
                } else {
                    // Move on to the next vector otherwise
                    mMapIterator++;
                    if (mMapIterator != mMapEndIterator) {
                        mNestedMapIterator = mMapIterator->second.begin();
                    }
                }
                return *this;
            }
            
            T& operator*() {
                return mNestedMapIterator->second.object;
            }

            T* operator->() {
                return &mNestedMapIterator->second.object;
            }
            
            const T& operator*() const {
                return mNestedMapIterator->second.object;
            }
            
            const T* operator->() const {
                return &mNestedMapIterator->second.object;
            }
            
            bool operator!=(const ForwardIterator& other) const {
                // Don't touch vector's itereator if we're at the end of unordered_map
                if (mMapIterator == mMapEndIterator) {
                    return mMapIterator != other.mMapIterator;
                } else {
                    return mMapIterator != other.mMapIterator || mNestedMapIterator != other.mCurrentSetIterator;
                }
            }
        };

#pragma mark - Private members
        
    private:
        std::mt19937 mEngine;
        std::uniform_real_distribution<float> mDistribution;
        std::unordered_map<Index, Bin> mBins;
        
        float mMinWeight = 0.0f;
        float mMaxWeight = 0.0f;
        float mTotalWeight = 0.0f;
        uint64_t mSize = 0;
        
        // Since unordered_set doesn't work with templates we replace it with unordered_map and use this counter as key
        uint64_t mInsertionCounter = 0;
        
        int32_t index(float weight) {
            // I believe that formula for index calculation presented in the article is incorrect
            // I changed it to log2f(weight / mMinWeight)
            return log2f(weight / mMinWeight);
        }
        
        float binMinWeight(Index index) {
            return mMinWeight * powf(2.0f, (float)index);
        }
        
        float binMaxWeight(Index index) {
            return mMinWeight * powf(2.0f, (float)(index + 1));
        }
        
#pragma mark - Public members
        
    public:
        LogarithmicBin(float minWeight, float maxWeight)
        :
        mMinWeight(minWeight),
        mMaxWeight(maxWeight),
        mEngine(std::random_device()()),
        mDistribution(0.0f, 1.0f)
        {
            if (maxWeight < minWeight) {
                throw std::invalid_argument(string_format("Maximum weight (%f) in LogarithmicBin must be larger than minimum weight (%f)!\n", maxWeight, minWeight));
            }
        }
        
        LogarithmicBin(float maxWeight) : LogarithmicBin(0.0f, maxWeight) { }
        
        float minWeight() const {
            return mMinWeight;
        }
        
        float maxWeight() const {
            return mMaxWeight;
        }
        
        float totalWeight() const {
            return mTotalWeight;
        }
        
        uint64_t size() const {
            return mSize;
        }
        
        bool empty() const {
            return mSize == 0;
        }
        
        void insert(const T& object, float weight) {
            if (weight < mMinWeight || weight > mMaxWeight) {
                throw std::invalid_argument(string_format("Weigth %f is outside of bounds [%f, %f]\n", weight, mMinWeight, mMaxWeight));
            }
            
            Index i = index(weight);
            auto& bin = mBins[i];
            bin.objects.insert(std::make_pair(mInsertionCounter, BinObject(object, weight)));
            bin.totalWeight += weight;
            mSize++;
            mInsertionCounter++;
            mTotalWeight += weight;
        }
        
        void erase(const ForwardIterator& it) {
            auto& bin = it.mMapIterator->second;
            mTotalWeight -= it.mNestedMapIterator->second.weight;
            bin.totalWeight -= it.mNestedMapIterator->second.weight;
            mSize--;
            bin.objects.erase(it.mNestedMapIterator);
            
            // Floating point errors may cause negative weights
            if (mTotalWeight < 0.0f) {
                mTotalWeight = 0.0f;
            }
            
            if (bin.totalWeight < 0.0f) {
                bin.totalWeight = 0.0f;
            }
        }

        ForwardIterator random() {
            float randomWeight = mDistribution(mEngine);
            float accumulatedWeight = 0.0f;

            Index randomBinIndex = 0;
            MapIterator randomBinIt;
            
            // Perform linear search of a random bin based on a probability
            // proportional to bin's total weight
            for (auto it = mBins.begin(); it != mBins.end(); ++it) {
                accumulatedWeight += it->second.totalWeight / mTotalWeight;
                
                if (it->second.objects.empty()) {
                    continue;
                }
                
                randomBinIndex = it->first;
                randomBinIt = it;
                
                if (randomWeight < accumulatedWeight) {
                    break;
                }
            }

            Index randomBinObjectIndex = 0;
            NestedMapIterator randomBinIterator;
            
            // Choose an object within the bin based on rejection sampling:
            // Pick an object at random within the bin and then accept it with probability At/Bmax,
            // where At is the object’s weight, and Bmax is the maximum weight of objects assigned to the bin.
            // Repeat until an object is accepted.
            //
            while (true) {
                randomBinObjectIndex = mDistribution(mEngine) * randomBinIt->second.objects.size();
                randomWeight = mDistribution(mEngine);
                randomBinIterator = std::next(randomBinIt->second.objects.begin(), randomBinObjectIndex);
                
                float probability = randomBinIterator->second.weight / binMaxWeight(randomBinIndex);
                
                if (randomWeight < probability) {
                    break;
                }
            }
            
            return ForwardIterator(randomBinIt, mBins.end(), randomBinIterator);
        }
        
#pragma mark Iteration
        
        ForwardIterator begin() {
            return ForwardIterator(mBins.begin(), mBins.end(), mBins.begin()->second.begin());
        }
        
        ForwardIterator end() {
            return ForwardIterator(mBins.end(), mBins.end(), mBins.end()->second.end());
        }
    };
    
    template<typename T>
    typename LogarithmicBin<T>::ForwardIterator begin(const LogarithmicBin<T>& bin) {
        return bin.begin();
    }
    
    template<typename T>
    typename LogarithmicBin<T>::ForwardIterator end(const LogarithmicBin<T>& bin) {
        return bin.end();
    }
    
}

#endif /* LogarithmicBin_hpp */
