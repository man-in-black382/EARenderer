//
//  SpatialHash.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 08.01.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SpatialHash_hpp
#define SpatialHash_hpp

#include "TupleHash.hpp"
#include "AxisAlignedBox3D.hpp"
#include "Macros.h"

#include <unordered_map>
#include <vector>
#include <glm/vec3.hpp>

namespace EARenderer {
    
    template <class T>
    class SpatialHash {
    private:
        
#pragma mark - Type aliases
        
        using Cell = std::tuple<int32_t, int32_t, int32_t>;
        using Objects = std::vector<T>;
        
#pragma mark - Forward iterator
        
        class iterator {
        private:
            friend SpatialHash;
            
            using MapIterator = typename std::unordered_map<Cell, Objects>::iterator;
            using VectorIterator = typename std::vector<T>::iterator;
            
            MapIterator mMapIterator;
            VectorIterator mCurrentVectorIterator;
            
            iterator(MapIterator i) : mMapIterator(i), mCurrentVectorIterator(i->second.begin()) { }
            iterator(MapIterator mapIterator, VectorIterator vectorIterator) : mMapIterator(mapIterator), mCurrentVectorIterator(vectorIterator) { }
            
        public:
            iterator& operator++() {
                // Increment iterator of current vector if it had not reached its end already
                if (mCurrentVectorIterator != mMapIterator->second.end()) {
                    mCurrentVectorIterator++;
                } else { // Move on to next vector otherwise
                    mMapIterator++;
                    mCurrentVectorIterator = mMapIterator->second.begin();
                }
                return *this;
            }

            T& operator*() {
                return *mCurrentVectorIterator;
            }

            bool operator!=(const iterator& other) const {
                return mMapIterator != other.mMapIterator || mCurrentVectorIterator != other.mCurrentVectorIterator;
            }

        };
        
#pragma mark - Range iterator
        
        class range_iterator {
            
        };

#pragma mark - Class private contents
        
        std::unordered_map<Cell, Objects> mObjects;
        AxisAlignedBox3D mBoundaries;
        uint32_t mResolution;
        
        int32_t cellIndex(int32_t axis, float positionOnAxis) const {
            float delta = mBoundaries.max[axis] - mBoundaries.min[axis];
            return (positionOnAxis - mBoundaries.min[axis]) / delta * mResolution;
        }
        
        Cell cell(const glm::vec3& position) const {
            return std::make_tuple(cellIndex(0, position.x), cellIndex(1, position.y), cellIndex(2, position.z));
        }

    public:
        
#pragma mark - Class public contents
        
#pragma mark Lifecycle
        
        SpatialHash(const AxisAlignedBox3D& boundaries, uint32_t resolution)
        :
        mBoundaries(boundaries),
        mResolution(resolution)
        {
//            mObjects[std::make_tuple(0, 0, 0)] = std::vector<T>();
        }
        
#pragma mark Modifiers
        
        void insert(const T& object, const glm::vec3& position) {
            ASSERT(mBoundaries.containsPoint(position), "Attempt to insert an object outside of spatial hash's boundaries");
            mObjects[cell(position)].push_back(object);
        }
        
#pragma mark Iteration
        
        iterator begin() {
            return iterator(mObjects.begin());
        }
        
        iterator end() {
            return mObjects.empty() ? begin() : iterator(mObjects.end(), mObjects.end()->second.end());
        }
    };
    
    template<typename T>
    typename SpatialHash<T>::iterator begin(const SpatialHash<T>& hash) {
        return hash.begin();
    }
    
    template<typename T>
    typename SpatialHash<T>::iterator end(const SpatialHash<T>& hash) {
        return hash.end();
    }
    
}

#endif /* SpatialHash_hpp */
