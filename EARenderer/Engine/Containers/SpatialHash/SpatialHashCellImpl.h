//
//  SpatialHashCellImpl.h
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SpatialHashCellImpl_h
#define SpatialHashCellImpl_h

namespace EARenderer {

#pragma mark - Lifecycle

    template <typename T>
    SpatialHash<T>::Cell::Cell(uint16_t x, uint16_t y, uint16_t z) {
        encodeX(x);
        encodeY(y);
        encodeZ(z);
    }

#pragma mark - Hash

    template <typename T>
    uint64_t
    SpatialHash<T>::Cell::hash() const {
        return mHash;
    }

#pragma mark - Setters

    template <typename T>
    void
    SpatialHash<T>::Cell::encodeX(uint16_t x) {
        mHash &= 0xFFFFFFFFFFFF0000;
        mHash |= x;
    }

    template <typename T>
    void
    SpatialHash<T>::Cell::encodeY(uint16_t x) {
        mHash &= 0xFFFFFFFF0000FFFF;
        uint64_t x64 = x;
        mHash |= (x64 << 16);
    }

    template <typename T>
    void
    SpatialHash<T>::Cell::encodeZ(uint16_t x) {
        mHash &= 0xFFFF0000FFFFFFFF;
        uint64_t x64 = x;
        mHash |= (x64 << 32);
    }

#pragma mark - Getters

    template <typename T>
    uint16_t
    SpatialHash<T>::Cell::decodeX() const {
        return mHash & 0xFFFF;
    }

    template <typename T>
    uint16_t
    SpatialHash<T>::Cell::decodeY() const {
        return (mHash >> 16) & 0xFFFF;
    }

    template <typename T>
    uint16_t
    SpatialHash<T>::Cell::decodeZ() const {
        return (mHash >> 32) & 0xFFFF;
    }

}

#endif /* SpatialHashCellImpl_h */
