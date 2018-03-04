//
//  SpatialHashRangeIteratorImpl.h
//  EARenderer
//
//  Created by Pavlo Muratov on 04.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SpatialHashRangeIteratorImpl_h
#define SpatialHashRangeIteratorImpl_h

namespace EARenderer {

#pragma mark - Lifecycle

    template <typename T>
    SpatialHash<T>::Range::Iterator::Iterator(const std::vector<CellBeginEndPair>& vectorIteratorPairs)
    :
    mCellBeginEndPairs(vectorIteratorPairs)
    { }

    template <typename T>
    void
    SpatialHash<T>::Range::Iterator::moveToEnd() {
        if (mCellBeginEndPairs.empty()) {
            return;
        }

        mCurrentPairIndex = mCellBeginEndPairs.size() - 1;
        mCellBeginEndPairs[mCurrentPairIndex].first = mCellBeginEndPairs[mCurrentPairIndex].second;
    }

#pragma mark - Operators

    template <typename T>
    typename SpatialHash<T>::Range::Iterator&
    SpatialHash<T>::Range::Iterator::operator++() {
        auto& pair = mCellBeginEndPairs[mCurrentPairIndex];
        auto& current = pair.first;
        auto& end = pair.second;

        if (current == end && mCurrentPairIndex == mCellBeginEndPairs.size() - 1) {
            throw std::out_of_range("Incrementing an iterator which had reached the end already");
        }

        current++;

        if (current == end) {
            if (mCurrentPairIndex != mCellBeginEndPairs.size() - 1) {
                mCurrentPairIndex++;
            }
        }

        return *this;
    }

    template <typename T>
    T&
    SpatialHash<T>::Range::Iterator::operator*() {
        return *(mCellBeginEndPairs[mCurrentPairIndex].first);
    }

    template <typename T>
    T*
    SpatialHash<T>::Range::Iterator::operator->() {
        return &(*(mCellBeginEndPairs[mCurrentPairIndex].first));
    }

    template <typename T>
    const T&
    SpatialHash<T>::Range::Iterator::operator*() const {
        return *mCellBeginEndPairs[mCurrentPairIndex].first;
    }

    template <typename T>
    const T*
    SpatialHash<T>::Range::Iterator::operator->() const {
        return &(*mCellBeginEndPairs[mCurrentPairIndex].first);
    }

    template <typename T>
    bool
    SpatialHash<T>::Range::Iterator::operator!=(const Iterator& other) const {
        bool indexNotEqual = mCurrentPairIndex != other.mCurrentPairIndex;

        if (mCellBeginEndPairs.empty()) {
            return indexNotEqual;
        } else {
            bool iteratorsNotEqual = mCellBeginEndPairs[mCurrentPairIndex].first != other.mCellBeginEndPairs[other.mCurrentPairIndex].first;
            return indexNotEqual || iteratorsNotEqual;
        }
    }

}

#endif /* SpatialHashRangeIteratorImpl_h */
