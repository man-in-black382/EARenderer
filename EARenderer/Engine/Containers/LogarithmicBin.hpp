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
#include "PackedLookupTable.hpp"

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
            PackedLookupTable<BinObject> objects;
            float totalWeight = 0.0f;
            
            Bin() : objects(1000) { }
        };
        
        using BinsIterator = typename std::unordered_map<Index, Bin>::iterator;
        using BinObjectsIterator = typename PackedLookupTable<BinObject>::Iterator;
        
#pragma mark Forward iterator
        
    public:
        class Iterator {
        private:
            friend LogarithmicBin;
            
            BinsIterator mBinsIterator;
            BinsIterator mBinsEndIterator;
            BinObjectsIterator mBinObjectsIterator;
            
            Iterator(BinsIterator i, BinsIterator endIterator, BinObjectsIterator binObjectsIterator)
            :
            mBinsIterator(i),
            mBinsEndIterator(endIterator),
            mBinObjectsIterator(binObjectsIterator)
            { }
            
        public:
            Iterator& operator++() {
                if (mBinsIterator == mBinsEndIterator) {
                    throw std::out_of_range("Incrementing an iterator which had reached the end already");
                }
                
                mBinObjectsIterator++;
                
                if (mBinObjectsIterator == mBinsIterator->second.objects.end()) {
                    mBinsIterator++;
                    if (mBinsIterator != mBinsEndIterator) {
                        mBinObjectsIterator = mBinsIterator->second.objects.begin();
                    }
                }
                
                return *this;
            }
            
            T& operator*() {
                ID id = *mBinObjectsIterator;
                Bin& bin = mBinsIterator->second;
                BinObject& binObject = bin.objects[id];
                return binObject.object;
            }

            T* operator->() {
                ID id = *mBinObjectsIterator;
                Bin& bin = mBinsIterator->second;
                BinObject& binObject = bin.objects[id];
                return &(binObject.object);
            }
            
            const T& operator*() const {
                ID id = *mBinObjectsIterator;
                Bin& bin = mBinsIterator->second;
                BinObject& binObject = bin.objects[id];
                return binObject.object;
            }
            
            const T* operator->() const {
                ID id = *mBinObjectsIterator;
                Bin& bin = mBinsIterator->second;
                BinObject& binObject = bin.objects[id];
                return &(binObject.object);
            }
            
            bool operator!=(const Iterator& other) const {
                // Don't touch vector's itereator if we're at the end of unordered_map
                if (mBinsIterator == mBinsEndIterator) {
                    return mBinsIterator != other.mMapIterator;
                } else {
                    return mBinsIterator != other.mMapIterator || mBinObjectsIterator != other.mNestedMapIterator;
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
            bin.objects.emplace(BinObject(object, weight));
            bin.totalWeight += weight;
            mTotalWeight += weight;
            mSize++;
        }
        
        void erase(const Iterator& it) {
            ID idToDelete = *it.mBinObjectsIterator;
            Bin& bin = it.mBinsIterator->second;
            BinObject& binObject = bin.objects[idToDelete];
            
            mTotalWeight -= binObject.weight;
            bin.totalWeight -= binObject.weight;
            mSize--;
            bin.objects.erase(idToDelete);
            
            // Floating point errors may cause negative weights
            if (mTotalWeight < 0.0f) {
                mTotalWeight = 0.0f;
            }
            
            if (bin.totalWeight < 0.0f) {
                bin.totalWeight = 0.0f;
            }
        }

        Iterator random() {
            float randomWeight = mDistribution(mEngine);
            float accumulatedWeight = 0.0f;

            Index randomBinIndex = 0;
            BinsIterator randomBinIterator;
            
            // Perform linear search of a random bin based on a probability
            // proportional to bin's total weight
            for (auto it = mBins.begin(); it != mBins.end(); ++it) {
                accumulatedWeight += it->second.totalWeight / mTotalWeight;
                
                if (it->second.objects.empty()) {
                    continue;
                }
                
                randomBinIndex = it->first;
                randomBinIterator = it;
                
                if (randomWeight < accumulatedWeight) {
                    break;
                }
            }

            Index randomBinObjectIndex = 0;
            BinObjectsIterator binObjectsIterator{ 0 };
            
            // Choose an object within the bin based on rejection sampling:
            // Pick an object at random within the bin and then accept it with probability At/Bmax,
            // where At is the object’s weight, and Bmax is the maximum weight of objects assigned to the bin.
            // Repeat until an object is accepted.
            //
            while (true) {
                auto& binObjects = randomBinIterator->second.objects;
                randomBinObjectIndex = mDistribution(mEngine) * (binObjects.size() - 1);
                randomWeight = mDistribution(mEngine);
                
                binObjectsIterator = binObjects.begin() + randomBinObjectIndex;
                BinObject& randomBinObject = binObjects[*binObjectsIterator];
                
                float probability = randomBinObject.weight / binMaxWeight(randomBinIndex);
                
                if (randomWeight < probability) {
                    break;
                }
            }
            
            Iterator i = Iterator(randomBinIterator, mBins.end(), binObjectsIterator);
            return i;
        }
        
#pragma mark Iteration
        
        Iterator begin() {
            return Iterator(mBins.begin(), mBins.end(), mBins.begin()->second.objects.begin());
        }
        
        Iterator end() {
            return Iterator(mBins.end(), mBins.end(), mBins.end()->second.objects.end());
        }
    };
    
    template<typename T>
    typename LogarithmicBin<T>::Iterator begin(const LogarithmicBin<T>& bin) {
        return bin.begin();
    }
    
    template<typename T>
    typename LogarithmicBin<T>::Iterator end(const LogarithmicBin<T>& bin) {
        return bin.end();
    }
    
}

#endif /* LogarithmicBin_hpp */
