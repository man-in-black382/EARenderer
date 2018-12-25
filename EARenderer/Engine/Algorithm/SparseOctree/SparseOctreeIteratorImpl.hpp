//
//  SparseOctreeIteratorImpl.h
//  EARenderer
//
//  Created by Pavlo Muratov on 04.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SparseOctreeIteratorImpl_h
#define SparseOctreeIteratorImpl_h

namespace EARenderer {

#pragma mark - Lifecycle

    template<typename T>
    SparseOctree<T>::Iterator::Iterator(MapIterator i, MapIterator endIterator)
            :
            mNodesIterator(i),
            mNodesEndIterator(endIterator) {
    }

    template<typename T>
    SparseOctree<T>::Iterator::Iterator(MapIterator endIterator)
            :
            mNodesIterator(endIterator),
            mNodesEndIterator(endIterator) {
    }

#pragma mark - Operators

    template<typename T>
    typename SparseOctree<T>::Iterator &
    SparseOctree<T>::Iterator::operator++() {
        if (mNodesIterator == mNodesEndIterator) {
            throw std::out_of_range("Incrementing an iterator which had reached the end already");
        }

        mNodesIterator++;

        return *this;
    }

    template<typename T>
    const typename SparseOctree<T>::Node &
    SparseOctree<T>::Iterator::operator*() const {
        return mNodesIterator->second;
    }

    template<typename T>
    const typename SparseOctree<T>::Node *
    SparseOctree<T>::Iterator::operator->() const {
        return &(mNodesIterator->second);
    }

    template<typename T>
    bool
    SparseOctree<T>::Iterator::operator!=(const Iterator &other) const {
        return mNodesIterator != other.mNodesIterator;
    }

}

#endif /* SparseOctreeIteratorImpl_h */
