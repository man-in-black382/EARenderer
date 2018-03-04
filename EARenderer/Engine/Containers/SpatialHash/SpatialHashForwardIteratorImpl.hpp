//
//  SpatialHashForwardIteratorImpl.h
//  EARenderer
//
//  Created by Pavlo Muratov on 04.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SpatialHashForwardIteratorImpl_h
#define SpatialHashForwardIteratorImpl_h

namespace EARenderer {

#pragma mark - Lifecycle

    template <typename T>
    SpatialHash<T>::ForwardIterator::ForwardIterator(MapIterator i, MapIterator endIterator)
    :
    mMapIterator(i),
    mMapEndIterator(endIterator)
    {
        if (i != endIterator) {
            mCurrentVectorIterator = i->second.begin();
        }
    }

#pragma mark - Operators

    template <typename T>
    typename SpatialHash<T>::ForwardIterator&
    SpatialHash<T>::ForwardIterator::operator++() {
        if (mMapIterator == mMapEndIterator) {
            throw std::out_of_range("Incrementing an iterator which had reached the end already");
        }

        mCurrentVectorIterator++;

        if (mCurrentVectorIterator == mMapIterator->second.end()) {
            mMapIterator++;
            if (mMapIterator != mMapEndIterator) {
                mCurrentVectorIterator = mMapIterator->second.begin();
            }
        }

        return *this;
    }

    template <typename T>
    T&
    SpatialHash<T>::ForwardIterator::operator*() {
        return *mCurrentVectorIterator;
    }

    template <typename T>
    T*
    SpatialHash<T>::ForwardIterator::operator->() {
        return &(*mCurrentVectorIterator);
    }

    template <typename T>
    const T&
    SpatialHash<T>::ForwardIterator::operator*() const {
        return *mCurrentVectorIterator;
    }

    template <typename T>
    const T*
    SpatialHash<T>::ForwardIterator::operator->() const {
        return &(*mCurrentVectorIterator);
    }

    template <typename T>
    bool
    SpatialHash<T>::ForwardIterator::operator!=(const ForwardIterator& other) const {
        // Don't touch vector's itereator if we're at the end of unordered_map
        if (mMapIterator == mMapEndIterator) {
            return mMapIterator != other.mMapIterator;
        } else {
            return mMapIterator != other.mMapIterator || mCurrentVectorIterator != other.mCurrentVectorIterator;
        }
    }

}

#endif /* SpatialHashForwardIteratorImpl_h */
