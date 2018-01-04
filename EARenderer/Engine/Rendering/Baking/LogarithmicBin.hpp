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
        std::uniform_real_distribution<float> mUniformDistribution;
        std::vector<BinIndex, Bin> mBins;
        
        float mMinWeight;
        float mMaxWeight;
        
        int32_t index(float weight) {
            return log2f((mMaxWeight - mMinWeight) / weight);
        }
        
    public:
        LogarithmicBin(float minWeight, float maxWeight)
        :
        mMinWeight(minWeight),
        mMaxWeight(maxWeight),
        mEngine(std::random_device()()),
        mUniformDistribution(mEngine)
        {
            ASSERT(maxWeight > minWeight, "Maximum weight in LogarithmicBin must be larger than minimum weight!");
        }
        
        LogarithmicBin(float maxWeight) : LogarithmicBin(0.0f, maxWeight) { }
        
        void insert(const T& object, float weight) {
            ASSERT(weight >= mMinWeight && weight <= mMaxWeight, "Weight " << weight << " is outside of bounds [" << mMinWeight << ", " << mMaxWeight << "]");
            auto& bin = mBins[index(weight)];
            bin.objects.push_back({ object, weight });
            bin.totalWeight += weight;
        }

        const T& random() const {
            float randomWeight = mUniformDistribution(mEngine);
            float accumulatedWeight = 0.0f;
            Bin randomBin;
            BinObject* randomBinObject = nullptr;
            
            for (int32_t i = 0; i < mBins.size(); i++) {
                Bin& bin = mBins[i];
                accumulatedWeight += bin.totalWeight;
                if (randomWeight > accumulatedWeight) {
                    randomBin = bin;
                    break;
                }
            }
            
            float weightDelta = mMaxWeight - mMinWeight;
            
            while (!randomBinObject) {
                int32_t randomObjectIndex = weightDelta / randomBin.objects.size();
                randomWeight = mUniformDistribution(mEngine);
                BinObject& rbo = randomBin.objects[randomObjectIndex];
                if (randomWeight < rbo.weight) {
                    randomBinObject = &rbo;
                }
            }
            
            return randomBinObject->object;
        }
    };
    
}

#endif /* LogarithmicBin_hpp */
