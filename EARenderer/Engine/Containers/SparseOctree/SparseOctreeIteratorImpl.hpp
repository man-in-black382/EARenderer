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

    template <typename T>
    SparseOctree<T>::Iterator::Iterator(MapIterator i, MapIterator endIterator, VectorIterator nodeObjectsIterator)
    :
    mNodesIterator(i),
    mNodesEndIterator(endIterator),
    mNodeObjectsIterator(nodeObjectsIterator)
    { }

    template <typename T>
    SparseOctree<T>::Iterator::Iterator(MapIterator endIterator)
    :
    mNodesIterator(endIterator),
    mNodesEndIterator(endIterator)
    { }

#pragma mark - Operators

    template <typename T>
    typename SparseOctree<T>::Iterator&
    SparseOctree<T>::Iterator::operator++() {
        if (mNodesIterator == mNodesEndIterator) {
            throw std::out_of_range("Incrementing an iterator which had reached the end already");
        }

        mNodeObjectsIterator++;

        if (mNodeObjectsIterator == mNodesIterator->second.objects.end()) {
            mNodesIterator++;
            if (mNodesIterator != mNodesEndIterator) {
                mNodeObjectsIterator = mNodesIterator->second.objects.begin();
            }
        }

        return *this;
    }

    template <typename T>
    std::pair<typename SparseOctree<T>::BoundingBoxRef, typename SparseOctree<T>::ObjectRef>
    SparseOctree<T>::Iterator::operator*() const {
        printf("Accessing node: %d, objects count %lu\n", mNodesIterator->first, mNodesIterator->second.objects.size());
        return std::make_pair(mNodesIterator->second.boundingBox,
                              *mNodeObjectsIterator);
    }

    template <typename T>
    std::pair<typename SparseOctree<T>::BoundingBoxRef, typename SparseOctree<T>::ObjectRef>
    SparseOctree<T>::Iterator::operator->() const {
        return std::make_pair(mNodesIterator->second.boundingBox,
                              *mNodeObjectsIterator);
    }

    template <typename T>
    bool
    SparseOctree<T>::Iterator::operator!=(const Iterator& other) const {
        // Don't touch vector's itereator if we're at the end of unordered_map
        if (mNodesIterator == mNodesEndIterator) {
            return mNodesIterator != other.mNodesIterator;
        } else {
            return mNodesIterator != other.mNodesIterator || mNodeObjectsIterator != other.mNodeObjectsIterator;
        }
    }

}

#endif /* SparseOctreeIteratorImpl_h */
