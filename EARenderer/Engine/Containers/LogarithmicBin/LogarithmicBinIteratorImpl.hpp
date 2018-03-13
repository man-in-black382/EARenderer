//
//  LogarithmicBinIteratorImpl.h
//  EARenderer
//
//  Created by Pavlo Muratov on 04.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef LogarithmicBinIteratorImpl_h
#define LogarithmicBinIteratorImpl_h

namespace EARenderer {

#pragma mark - Lifecycle

    template <typename T>
    LogarithmicBin<T>::Iterator::Iterator(BinsIterator i, BinsIterator endIterator, BinObjectsIterator binObjectsIterator)
    :
    mBinsIterator(i),
    mBinsEndIterator(endIterator),
    mBinObjectsIterator(binObjectsIterator)
    { }

    template <typename T>
    LogarithmicBin<T>::Iterator::Iterator(BinsIterator endIterator)
    :
    mBinsIterator(endIterator),
    mBinsEndIterator(endIterator)
    { }

#pragma mark - Operators

    template <typename T>
    typename LogarithmicBin<T>::Iterator&
    LogarithmicBin<T>::Iterator::operator++() {
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

    template <typename T>
    T&
    LogarithmicBin<T>::Iterator::operator*() {
        ID id = *mBinObjectsIterator;
        Bin& bin = mBinsIterator->second;
        BinObject& binObject = bin.objects[id];
        return binObject.object;
    }

    template <typename T>
    T*
    LogarithmicBin<T>::Iterator::operator->() {
        ID id = *mBinObjectsIterator;
        Bin& bin = mBinsIterator->second;
        BinObject& binObject = bin.objects[id];
        return &(binObject.object);
    }

    template <typename T>
    const T&
    LogarithmicBin<T>::Iterator::operator*() const {
        ID id = *mBinObjectsIterator;
        Bin& bin = mBinsIterator->second;
        BinObject& binObject = bin.objects[id];
        return binObject.object;
    }

    template <typename T>
    const T*
    LogarithmicBin<T>::Iterator::operator->() const {
        ID id = *mBinObjectsIterator;
        Bin& bin = mBinsIterator->second;
        BinObject& binObject = bin.objects[id];
        return &(binObject.object);
    }

    template <typename T>
    bool
    LogarithmicBin<T>::Iterator::operator!=(const Iterator& other) const {
        // Don't touch vector's itereator if we're at the end of unordered_map
        if (mBinsIterator == mBinsEndIterator) {
            return mBinsIterator != other.mBinsIterator;
        } else {
            return mBinsIterator != other.mBinsIterator || mBinObjectsIterator != other.mBinObjectsIterator;
        }
    }

}

#endif /* LogarithmicBinIteratorImpl_h */
