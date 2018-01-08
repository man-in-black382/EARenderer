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
    public:
        struct BinObject {
            T object;
            float weight = 0.0f;
        };
        
        struct Bin {
            std::vector<BinObject> objects;
            float totalWeight = 0.0f;
        };

    private:
        using BinIndex = int32_t;
        
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
            return mMinWeight > 0.0f ? log2f(weight / mMinWeight) : log2f(weight);
        }
        
        float binMinWeight(int32_t index) {
            return mMinWeight * powf(2.0f, (float)index);
        }
        
        float binMaxWeight(int32_t index) {
            return mMinWeight * powf(2.0f, (float)(index + 1));
        }
        
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

        const T& random() {
            float randomWeight = mDistribution(mEngine);
            float accumulatedWeight = 0.0f;
            
            int32_t randomBinIndex = 0;
            const Bin* randomBin = nullptr;
            const BinObject* randomBinObject = nullptr;
            
            // Perform linear search of a random bin based on a probability
            // proportional to bin's total weight
            for (auto& indexBinPair : mBins) {
                accumulatedWeight += indexBinPair.second.totalWeight / mTotalWeight;
                if (randomWeight < accumulatedWeight) {
                    randomBinIndex = indexBinPair.first;
                    randomBin = &indexBinPair.second;
                    break;
                }
            }
            
            // Choose an object within the bin based on rejection sampling:
            // Pick an object at random within the bin and then accept it with probability At/Bmax,
            // where At is the object’s weight, and Bmax is the maximum weight of objects assigned to the bin.
            // Repeat until an object is accepted.
            //
            while (!randomBinObject) {
                int32_t randomObjectIndex = mDistribution(mEngine) * randomBin->objects.size();
                randomWeight = mDistribution(mEngine);
                const BinObject& rbo = randomBin->objects[randomObjectIndex];
                float probability = rbo.weight / binMaxWeight(randomBinIndex);

                if (randomWeight < probability) {
                    randomBinObject = &rbo;
                }
            }
            
            return randomBinObject->object;
        }
    };
    
}

#endif /* LogarithmicBin_hpp */
