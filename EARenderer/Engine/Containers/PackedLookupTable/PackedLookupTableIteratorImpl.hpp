//
//  PackedLookupTableIteratorImpl.h
//  EARenderer
//
//  Created by Pavlo Muratov on 04.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef PackedLookupTableIteratorImpl_h
#define PackedLookupTableIteratorImpl_h

namespace EARenderer {

    template <typename T>
    PackedLookupTable<T>::Iterator::Iterator(ID* in)
    :
    mCurrentObjectID(in)
    { }

    template <typename T>
    typename PackedLookupTable<T>::Iterator&
    PackedLookupTable<T>::Iterator::operator++() {
        mCurrentObjectID++;
        return *this;
    }

    template <typename T>
    typename PackedLookupTable<T>::Iterator&
    PackedLookupTable<T>::Iterator::operator+=(size_t offset) {
        mCurrentObjectID += offset;
        return *this;
    }

    template <typename T>
    typename PackedLookupTable<T>::Iterator
    PackedLookupTable<T>::Iterator::operator+(size_t offset) {
        Iterator copy(*this);
        copy += offset;
        return copy;
    }

    template <typename T>
    ID
    PackedLookupTable<T>::Iterator::operator*() const {
        return *mCurrentObjectID;
    }

    template <typename T>
    bool
    PackedLookupTable<T>::Iterator::operator!=(const Iterator& other) const {
        return mCurrentObjectID != other.mCurrentObjectID;
    }

}

#endif /* PackedLookupTableIteratorImpl_h */
