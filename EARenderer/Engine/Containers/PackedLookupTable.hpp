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
    
    typedef uint32_t ID;
    static ID IDNotFound = 0;
    
    template <typename T>
    class PackedLookupTable {
    private:
        
#pragma mark - Private
        
        struct Allocation {
            ID objectID;
            uint16_t objectIndex;
            uint16_t nextAllocationIndex;
        };
        
        // Used to extract the allocation index from an object id
        static const uint16_t ObjectIDIndexMask = 0xFFFF;
        
        // Used to mark an allocation as owning no object
        static const uint16_t DeadObjectIndex = 0xFFFF;
        
        // Storage for objects
        // Objects are contiguous, and always packed to the start of the storage.
        // Objects can be relocated in this storage thanks to the separate list of allocations.
        size_t mObjectsCount = 0;
        uint16_t mCapacity = 0;
        uint16_t mMaxCapacity = std::numeric_limits<uint16_t>::max();
        T* mObjects = nullptr;
        
        // The allocation ID of each object in the object array (1-1 mapping)
        ID* mObjectIDs = nullptr;
        
        // FIFO queue to allocate objects with least ID reuse possible
        Allocation* mAllocations = nullptr;
        
        // Last possible free allocation slot
        uint16_t mLastAllocationIndex = -1;
        
        // The next index struct to use for an allocation
        uint16_t mNextAllocationIndex = -1;
        
        Allocation* popFreeAllocation() {
            // Reserve more memory if needed
            if (mObjectsCount == mCapacity) {
                reserve(mCapacity * 2);
                mAllocations[mLastAllocationIndex].nextAllocationIndex = mObjectsCount;
            }
            
            assert(mObjectsCount < mCapacity);
            
            // pop an allocation from the FIFO
            Allocation* allocation = &mAllocations[mNextAllocationIndex];
            mNextAllocationIndex = allocation->nextAllocationIndex;
            
            // increment the allocation count in the 16 MSBs without modifying the allocation's index (in the 16 LSBs)
            allocation->objectID += 0x10000;
            
            // always allocate the object at the end of the storage
            allocation->objectIndex = mObjectsCount;
            mObjectsCount++;
            
            // update reverse-lookup so objects can know their ID
            mObjectIDs[allocation->objectIndex] = allocation->objectID;
            
            return allocation;
        }
        
    public:
        
#pragma mark - Public
#pragma mark Iterator
        
        struct Iterator {
        private:
            ID* mCurrentObjectID;
            
        public:
            Iterator(ID* in) {
                mCurrentObjectID = in;
            }
            
            Iterator& operator++() {
                mCurrentObjectID++;
                return *this;
            }
            
            ID operator*() {
                return *mCurrentObjectID;
            }
            
            bool operator!=(const Iterator& other) const {
                return mCurrentObjectID != other.mCurrentObjectID;
            }
        };

#pragma mark - Lifecycle
        
        PackedLookupTable(size_t capacity) {
            // -1 because index 0xFFFF is reserved as a tombstone
            assert(capacity < 0x10000 - 1);
            
            mObjectsCount = 0;
            mCapacity = capacity;
            
            mObjects = reinterpret_cast<T*>(new char[capacity * sizeof(T)]);
            assert(mObjects);
            
            mObjectIDs = new ID[mCapacity];
            assert(mObjectIDs);
            
            mAllocations = new Allocation[mCapacity];
            assert(mAllocations);
            
            for (size_t i = 0; i < mCapacity; i++) {
                mAllocations[i].objectID = static_cast<ID>(i);
                mAllocations[i].objectIndex = DeadObjectIndex;
                mAllocations[i].nextAllocationIndex = (uint16_t)(i + 1);
            }
            
            if (mCapacity > 0) {
                mAllocations[mCapacity - 1].nextAllocationIndex = 0;
            }
            
            mLastAllocationIndex = mCapacity - 1;
            mNextAllocationIndex = 0;
        }
        
        PackedLookupTable(const PackedLookupTable& other) {
            
            mObjectsCount = other.mObjectsCount;
            mCapacity = other.mCapacity;
            
            mObjects = reinterpret_cast<T*>(new char[other.mCapacity * sizeof(T)]);
            assert(mObjects);
            
            mObjectIDs = new uint32_t[other.mCapacity];
            assert(mObjectIDs);
            
            mAllocations = new Allocation[other.mCapacity];
            assert(mAllocations);
            
            for (size_t i = 0; i < other.mCapacity; i++) {
                new (mObjects + i) T(*(other.mObjects + i));
                mObjectIDs[i] = other.mObjectIDs[i];
            }
            
            for (size_t i = 0; i < other.mCapacity; i++) {
                mAllocations[i] = other.mAllocations[i];
            }
            
            mLastAllocationIndex = other.mLastAllocationIndex;
            mNextAllocationIndex = other.mNextAllocationIndex;
        }
        
        PackedLookupTable(PackedLookupTable&& other) {
            swap(other);
            
            other.mObjects = nullptr;
            other.mObjectIDs = nullptr;
            other.mAllocations = nullptr;
        }
        
        ~PackedLookupTable() {
            for (size_t i = 0; i < mCapacity; i++) {
                mObjects[i].~T();
            }
            delete[] ((char*)mObjects);
            delete[] mObjectIDs;
            delete[] mAllocations;
        }
        
        void swap(PackedLookupTable& other) {
            std::swap(mObjectsCount, other.mObjectsCount);
            std::swap(mCapacity, other.mCapacity);
            std::swap(mObjects, other.mObjects);
            std::swap(mObjectIDs, other.mObjectIDs);
            std::swap(mAllocations, other.mAllocations);
            std::swap(mLastAllocationIndex, other.mLastAllocationIndex);
            std::swap(mNextAllocationIndex, other.mNextAllocationIndex);
        }
        
#pragma mark - Operators
        
        PackedLookupTable& operator=(PackedLookupTable rhs) {
            swap(rhs);
            return *this;
        }
        
        T& operator[](ID id) const {
            // grab the allocation corresponding to this ID
            Allocation* allocation = &mAllocations[id & ObjectIDIndexMask];
            
            // grab the object associated with the allocation
            return *(mObjects + allocation->objectIndex);
        }
        
#pragma mark - Modifiers
        
        ID insert(const T& object) {
            Allocation* allocation = popFreeAllocation();
            T* insertionPointer = mObjects + allocation->objectIndex;
            new (insertionPointer) T(object);
            return allocation->objectID;
        }
        
        ID insert(T&& object) {
            Allocation* allocation = popFreeAllocation();
            T* insertionPointer = mObjects + allocation->objectIndex;
            new (insertionPointer) T(std::move(object));
            return allocation->objectID;
        }
        
        template<class... Args>
        ID emplace(Args&&... args) {
            Allocation* allocation = popFreeAllocation();
            T* o = mObjects + allocation->objectIndex;
            new (o) T(std::forward<Args>(args)...);
            return allocation->objectID;
        }
        
        bool contains(ID id) const {
            // grab the allocation by grabbing the allocation index from the id's LSBs
            Allocation* allocation = &mAllocations[id & ObjectIDIndexMask];
            
            // * NON-conservative test that the IDs match (if the allocation has been reused 2^16 times, it'll loop over)
            // * Also check that the object is hasn't already been deallocated.
            // * This'll prevent an object that was just freed from appearing to be contained, but still doesn't disambiguate between two objects with the same ID (see first bullet point)
            return allocation->objectID == id && allocation->objectIndex != DeadObjectIndex;
        }
        
        void erase(ID id) {
            if (!contains(id)) {
                throw std::invalid_argument(string_format("There is no object with ID %d\n", id));
            }
            
            // grab the allocation to delete
            Allocation* allocation = &mAllocations[id & ObjectIDIndexMask];
            
            // grab the object for this allocation
            T* object = mObjects + allocation->objectIndex;
            
            // if necessary, move (aka swap) the last object into the location of the object to erase, then unconditionally delete the last object
            if (allocation->objectIndex != mObjectsCount - 1) {
                T* last = mObjects + mObjectsCount - 1;
                *object = std::move(*last);
                object = last;
                
                // since the last object was moved into the deleted location, the associated object ID array's value must also be moved similarly
                ID lastObjectID = mObjectIDs[mObjectsCount - 1];
                mObjectIDs[allocation->objectIndex] = lastObjectID;
                
                // since the last object has changed location, its allocation needs to be updated to the new location.
                uint16_t lastAllocationIndex = lastObjectID & ObjectIDIndexMask;
                mAllocations[lastAllocationIndex].objectIndex = allocation->objectIndex;
            }
            
            // destroy the removed object and pop it from the array
            object->~T();
            mObjectsCount--;
            
            // push the deleted allocation onto the FIFO
            mAllocations[mLastAllocationIndex].nextAllocationIndex = allocation->objectID & ObjectIDIndexMask;
            mLastAllocationIndex = allocation->objectID & ObjectIDIndexMask;
            
            // put a tombstone where the allocation used to point to an object index
            allocation->objectIndex = DeadObjectIndex;
        }
        
        void reserve(uint16_t capacity) {
            if (mCapacity == mMaxCapacity) {
                throw std::length_error(string_format("Cannot reserve more memory. Maximum capacity (%d) has already been reached\n", mMaxCapacity));
            }
            
            uint16_t newCapacity = std::min(capacity, mMaxCapacity);

            T* objects = reinterpret_cast<T*>(new char[newCapacity * sizeof(T)]);
            ID* objectIDs = new ID[newCapacity];
            Allocation* allocations = new Allocation[newCapacity];
            
            for (size_t i = 0; i < mObjectsCount; ++i) {
                objects[i] = std::move(mObjects[i]);
                mObjects[i].~T();
            }
            
            memcpy(objectIDs, mObjectIDs, mObjectsCount * sizeof(ID));
            memcpy(allocations, mAllocations, mCapacity * sizeof(Allocation));
            
            for (size_t i = mCapacity; i < newCapacity; ++i) {
                allocations[i].objectID = static_cast<ID>(i);
                allocations[i].objectIndex = DeadObjectIndex;
                allocations[i].nextAllocationIndex = (uint16_t)(i + 1);
            }
            
            // Link last allocation to the new memory chunk
            allocations[mLastAllocationIndex].nextAllocationIndex = mCapacity;
            allocations[newCapacity - 1].nextAllocationIndex = mLastAllocationIndex;
            mNextAllocationIndex = mObjectsCount;
            
            delete [] mObjects;
            delete [] mObjectIDs;
            delete [] mAllocations;
            
            mObjects = objects;
            mObjectIDs = objectIDs;
            mAllocations = allocations;
            
            mCapacity = newCapacity;
        }
        
#pragma mark - Accessors
        
        bool empty() const {
            return mObjectsCount == 0;
        }
        
        size_t size() const {
            return mObjectsCount;
        }
        
        size_t capacity() const {
            return mCapacity;
        }
        
#pragma mark - Iteration
        
        Iterator begin() const {
            return Iterator{ mObjectIDs };
        }
        
        Iterator end() const {
            return Iterator{ mObjectIDs + mObjectsCount };
        }
    };
    
    template<typename T>
    typename PackedLookupTable<T>::Iterator begin(const PackedLookupTable<T>& fl)
    {
        return fl.begin();
    }
    
    template<typename T>
    typename PackedLookupTable<T>::Iterator end(const PackedLookupTable<T>& fl)
    {
        return fl.end();
    }
    
    template<typename T>
    void swap(PackedLookupTable<T>& lhs, PackedLookupTable<T>& rhs)
    {
        lhs.swap(rhs);
    }

}

#endif /* IDLookupTable_hpp */
