//
//  LogarithmicBin.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 1/4/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef LogarithmicBin_hpp
#define LogarithmicBin_hpp

#include <vector>
#include <unordered_map>
#include <random>

#include "Macros.h"

namespace EARenderer {
    
    // Based on paper http://www.cg.tuwien.ac.at/research/publications/2009/cline-09-poisson/cline-09-poisson-paper.pdf
    
    template <class T>
    class LogarithmicBin {
    private:
        
#pragma mark - Nested types
        
        struct BinObject {
            T object;
            float weight = 0.0f;
        };
    	
        struct Bin {
            std::vector<BinObject> objects;
            float totalWeight = 0.0f;
        };
        
        using BinIndex = int32_t;
        using MapIterator = typename std::unordered_map<BinIndex, Bin>::iterator;
        using VectorIterator = typename std::vector<BinObject>::iterator;
        
#pragma mark Forward iterator
        
    public:
        class ForwardIterator {
        private:
            friend LogarithmicBin;
            
            MapIterator mMapIterator;
            MapIterator mMapEndIterator;
            VectorIterator mCurrentVectorIterator;
            
            ForwardIterator(MapIterator i, MapIterator endIterator, size_t advance = 0)
            :
            mMapIterator(i),
            mMapEndIterator(endIterator)
            {
                if (i != endIterator) {
                    mCurrentVectorIterator = i->second.objects.begin() + advance;
                }
            }
            
        public:
            ForwardIterator& operator++() {
                if (mMapIterator == mMapEndIterator) {
                    throw std::out_of_range("Incrementing an iterator which had reached the end already");
                }
                
                if (mCurrentVectorIterator != mMapIterator->second.end()) {
                    // Increment iterator of current vector if it had not reached its end already
                    mCurrentVectorIterator++;
                } else {
                    // Move on to the next vector otherwise
                    mMapIterator++;
                    if (mMapIterator != mMapEndIterator) {
                        mCurrentVectorIterator = mMapIterator->second.begin();
                    }
                }
                return *this;
            }
            
            T& operator*() {
                return mCurrentVectorIterator->object;
            }
            
            T* operator->() {
                return &mCurrentVectorIterator->object;
            }
            
            const T& operator*() const {
                return mCurrentVectorIterator->object;
            }
            
            const T* operator->() const {
                return &mCurrentVectorIterator->object;
            }
            
            bool operator!=(const ForwardIterator& other) const {
                // Don't touch vector's itereator if we're at the end of unordered_map
                if (mMapIterator == mMapEndIterator) {
                    return mMapIterator != other.mMapIterator;
                } else {
                    return mMapIterator != other.mMapIterator || mCurrentVectorIterator != other.mCurrentVectorIterator;
                }
            }
        };

#pragma mark - Private members
        
    private:
        std::mt19937 mEngine;
        std::uniform_real_distribution<float> mDistribution;
        std::unordered_map<BinIndex, Bin> mBins;
        
        float mMinWeight = 0.0f;
        float mMaxWeight = 0.0f;
        float mTotalWeight = 0.0f;
        uint64_t mSize = 0;
        
        int32_t index(float weight) {
            // I believe that formula for index calculation presented in the article is incorrect
            // I changed it to log2f(weight / mMinWeight)
            return mMinWeight == 0.0f ? log2f(weight) : log2f(weight / mMinWeight);
        }
        
        float binMinWeight(int32_t index) {
            return mMinWeight * powf(2.0f, (float)index);
        }
        
        float binMaxWeight(int32_t index) {
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
            ASSERT(maxWeight >= minWeight, "Maximum weight in LogarithmicBin must be larger than minimum weight!");
        }
        
        LogarithmicBin(float maxWeight) : LogarithmicBin(0.0f, maxWeight) { }
        
        float totalWeight() const {
            return mTotalWeight;
        }
        
        uint64_t size() const {
            return mSize;
        }
        
        void insert(const T& object, float weight) {
            ASSERT(weight >= mMinWeight && weight <= mMaxWeight, "Weight " << weight << " is outside of bounds [" << mMinWeight << ", " << mMaxWeight << "]");
            
            auto& bin = mBins[index(weight)];
            bin.objects.push_back({ object, weight });
            bin.totalWeight += weight;
            mSize++;
            mTotalWeight += weight;
        }

        ForwardIterator random() {
            float randomWeight = mDistribution(mEngine);
            float accumulatedWeight = 0.0f;
            
            int32_t randomBinIndex = 0;
            MapIterator randomBinIt;
            
            // Perform linear search of a random bin based on a probability
            // proportional to bin's total weight
            for (auto it = mBins.begin(); it != mBins.end(); ++it) {
                accumulatedWeight += it->second.totalWeight / mTotalWeight;
                
                if (randomWeight < accumulatedWeight) {
                    randomBinIndex = it->first;
                    randomBinIt = it;
                    break;
                }
            }
            
            int32_t randomBinObjectIndex = 0;
            const BinObject* randomBinObject = nullptr;
            
            // Choose an object within the bin based on rejection sampling:
            // Pick an object at random within the bin and then accept it with probability At/Bmax,
            // where At is the object’s weight, and Bmax is the maximum weight of objects assigned to the bin.
            // Repeat until an object is accepted.
            //
            while (!randomBinObject) {
                randomBinObjectIndex = mDistribution(mEngine) * randomBinIt->second.objects.size();
                randomWeight = mDistribution(mEngine);
                const BinObject& rbo = randomBinIt->second.objects[randomBinObjectIndex];
                float probability = rbo.weight / binMaxWeight(randomBinIndex);

                if (randomWeight < probability) {
                    randomBinObject = &rbo;
                }
            }
            
            return ForwardIterator(randomBinIt, mBins.end(), randomBinObjectIndex);
        }
        
#pragma mark Iteration
        
        ForwardIterator begin() {
            return ForwardIterator(mBins.begin(), mBins.end());
        }
        
        ForwardIterator end() {
            return ForwardIterator(mBins.end(), mBins.end());
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
