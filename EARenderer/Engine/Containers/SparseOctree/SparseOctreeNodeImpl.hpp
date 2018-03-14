//
//  SparseOctreeNodeImpl.h
//  EARenderer
//
//  Created by Pavlo Muratov on 04.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SparseOctreeNodeImpl_h
#define SparseOctreeNodeImpl_h

namespace EARenderer {

    template <typename T>
    void
    SparseOctree<T>::Node::setChildPresent(BitMask childIndex, bool isPresent) {
        uint16_t mask = 1 << (childIndex + 8);

        if (isPresent) {
            mChildInfo |= mask;
        } else {
            mChildInfo &= ~mask;
        }
    }

    template <typename T>
    void
    SparseOctree<T>::Node::setChildLeaf(BitMask childIndex, bool isLeaf) {
        uint16_t mask = 1 << childIndex;

        if (isLeaf) {
            mChildInfo |= mask;
        } else {
            mChildInfo &= ~mask;
        }
    }

    template <typename T>
    bool
    SparseOctree<T>::Node::isChildPresent(BitMask childIndex) const {
        uint16_t mask = 1 << (childIndex + 8);
        return mChildInfo & mask;
    }

    template <typename T>
    bool
    SparseOctree<T>::Node::isChildLeaf(BitMask childIndex) const {
        uint16_t mask = 1 << childIndex;
        return mChildInfo & mask;
    }

    template <typename T>
    const AxisAlignedBox3D&
    SparseOctree<T>::Node::boundingBox() const {
        return mBoundingBox;
    }

    template <typename T>
    const std::vector<T>&
    SparseOctree<T>::Node::objects() const {
        return mObjects;
    }

}

#endif /* SparseOctreeNodeImpl_h */
