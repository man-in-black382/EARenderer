//
//  LogarithmicBinImpl.h
//  EARenderer
//
//  Created by Pavlo Muratov on 04.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef LogarithmicBinImpl_h
#define LogarithmicBinImpl_h

namespace EARenderer {

#pragma mark - Lifecycle

    template <typename T>
    LogarithmicBin<T>::LogarithmicBin::LogarithmicBin(float minWeight, float maxWeight)
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

    template <typename T>
    LogarithmicBin<T>::LogarithmicBin::LogarithmicBin(float maxWeight)
    :
    LogarithmicBin(0.0f, maxWeight)
    { }

#pragma mark - Accessors

    template <typename T>
    int32_t
    LogarithmicBin<T>::index(float weight) {
        // I believe that formula for index calculation presented in the article is incorrect
        // I changed it to log2f(weight / mMinWeight)
        return log2f(weight / mMinWeight);
    }

    template <typename T>
    float
    LogarithmicBin<T>::binMinWeight(Index index) {
        return mMinWeight * powf(2.0f, (float)index);
    }

    template <typename T>
    float
    LogarithmicBin<T>::binMaxWeight(Index index) {
        return mMinWeight * powf(2.0f, (float)(index + 1));
    }

    template <typename T>
    float
    LogarithmicBin<T>::minWeight() const {
        return mMinWeight;
    }

    template <typename T>
    float
    LogarithmicBin<T>::maxWeight() const {
        return mMaxWeight;
    }

    template <typename T>
    float
    LogarithmicBin<T>::totalWeight() const {
        return mTotalWeight;
    }

    template <typename T>
    uint64_t
    LogarithmicBin<T>::size() const {
        return mSize;
    }

    template <typename T>
    bool
    LogarithmicBin<T>::empty() const {
        return mSize == 0;
    }

    template <typename T>
    void
    LogarithmicBin<T>::insert(const T& object, float weight) {
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

    template <typename T>
    void
    LogarithmicBin<T>::erase(const Iterator& it) {
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

    template <typename T>
    typename LogarithmicBin<T>::Iterator
    LogarithmicBin<T>::random() {
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

    template <typename T>
    typename LogarithmicBin<T>::Iterator
    LogarithmicBin<T>::begin() {
        return Iterator(mBins.begin(), mBins.end(), mBins.begin()->second.objects.begin());
    }

    template <typename T>
    typename LogarithmicBin<T>::Iterator
    LogarithmicBin<T>::end() {
        return Iterator(mBins.end(), mBins.end(), mBins.end()->second.objects.end());
    }

}

#endif /* LogarithmicBinImpl_h */
