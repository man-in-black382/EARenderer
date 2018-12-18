//
//  IDLookupTable.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 28.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef IDLookupTable_hpp
#define IDLookupTable_hpp

#include <stdlib.h>
#include <utility>
#include <assert.h>
#include <stdexcept>
#include <limits>

#include "StringUtils.hpp"

// http://bitsquid.blogspot.ca/2011/09/managing-decoupling-part-4-id-lookup.html

namespace EARenderer {

    typedef uint64_t ID;
    static ID IDNotFound = 0;

    template<typename T>
    class PackedLookupTable {
    private:

#pragma mark - Private

        struct Allocation {
            ID objectID;
            uint64_t objectIndex;
            uint64_t nextAllocationIndex;
        };

        // Used to mark an allocation as owning no object
        static const uint64_t DeadObjectIndex = -1;

        // Used to mark an allocation as owning no object
        static const uint64_t NotAnIndex = -1;

        // Storage for objects
        // Objects are contiguous, and always packed to the start of the storage.
        // Objects can be relocated in this storage thanks to the separate list of allocations.
        size_t mObjectsCount = 0;
        uint64_t mCapacity = 0;
        uint64_t mMaxCapacity = std::numeric_limits<uint64_t>::max();
        T *mObjects = nullptr;

        // The allocation ID of each object in the object array (1-1 mapping)
        ID *mObjectIDs = nullptr;

        // FIFO queue to allocate objects
        Allocation *mAllocations = nullptr;

        // Last possible free allocation slot
        uint64_t mLastAllocationIndex = NotAnIndex;

        // The next index struct to use for an allocation
        uint64_t mNextAllocationIndex = NotAnIndex;

        Allocation *popFreeAllocation();

    public:

#pragma mark - Public
#pragma mark Iterator

        struct Iterator {
        private:
            ID *mCurrentObjectID;

        public:
            Iterator(ID *in);

            Iterator &operator++();

            Iterator &operator+=(size_t offset);

            Iterator operator+(size_t offset);

            ID operator*() const;

            bool operator!=(const Iterator &other) const;
        };

#pragma mark - Lifecycle

        PackedLookupTable(size_t capacity);

        PackedLookupTable(const PackedLookupTable &other);

        PackedLookupTable(PackedLookupTable &&other);

        ~PackedLookupTable();

        void swap(PackedLookupTable &other);

#pragma mark - Operators

        PackedLookupTable &operator=(PackedLookupTable rhs);

        T &operator[](ID id);

        const T &operator[](ID id) const;

#pragma mark - Modifiers

        ID insert(const T &object);

        ID insert(T &&object);

        template<class... Args>
        ID emplace(Args &&... args);

        bool contains(ID id) const;

        void erase(ID id);

        void reserve(uint64_t capacity);

#pragma mark - Accessors

        bool empty() const;

        size_t size() const;

        size_t capacity() const;

        const T *data() const;

#pragma mark - Iteration

        Iterator begin() const;

        Iterator end() const;
    };

    template<typename T>
    typename PackedLookupTable<T>::Iterator begin(const PackedLookupTable<T> &fl) {
        return fl.begin();
    }

    template<typename T>
    typename PackedLookupTable<T>::Iterator end(const PackedLookupTable<T> &fl) {
        return fl.end();
    }

    template<typename T>
    void swap(PackedLookupTable<T> &lhs, PackedLookupTable<T> &rhs) {
        lhs.swap(rhs);
    }

}

#include "PackedLookupTableImpl.hpp"
#include "PackedLookupTableIteratorImpl.hpp"

#endif /* IDLookupTable_hpp */
