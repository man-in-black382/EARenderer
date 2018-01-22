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

#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <limits>
#include <glm/vec3.hpp>

namespace EARenderer {
    
    template <class T>
    class SpatialHash {
    private:
        
#pragma mark - Type aliases
        
        using Cell = std::tuple<int32_t, int32_t, int32_t>;
        using Objects = std::vector<T>;
        
#pragma mark - Nested types
#pragma mark Forward iterator
        
        class ForwardIterator {
        private:
            friend SpatialHash;
            
            using MapIterator = typename std::unordered_map<Cell, Objects>::iterator;
            using VectorIterator = typename std::vector<T>::iterator;
            
            MapIterator mMapIterator;
            MapIterator mMapEndIterator;
            VectorIterator mCurrentVectorIterator;
            
            ForwardIterator(MapIterator i, MapIterator endIterator)
            :
            mMapIterator(i),
            mMapEndIterator(endIterator)
            {
                if (i != endIterator) {
                    mCurrentVectorIterator = i->second.begin();
                }
            }
            
        public:
            ForwardIterator& operator++() {
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

            T& operator*() {
                return *mCurrentVectorIterator;
            }
            
            T* operator->() {
                return &(*mCurrentVectorIterator);
            }
            
            const T& operator*() const {
                return *mCurrentVectorIterator;
            }
            
            const T* operator->() const {
                return &(*mCurrentVectorIterator);
            }

            bool operator!=(const ForwardIterator& other) const {
                // Don't touch vector's itereator if we're at the end of unordered_map
                if (mMapIterator == mMapEndIterator) {
                    return mMapIterator != other.mMapIterator;
                } else {
                    return mMapIterator != other.mMapIterator || mCurrentVectorIterator != other.mCurrentVectorIterator;
                }
            }
        };
        
#pragma mark Range iterator

        class Range {
        public:
            
            class Iterator {
            private:
                friend SpatialHash;
                friend Range;
                
                using CellObjectsIterator = typename std::vector<T>::iterator;
                using CellBeginEndPair = std::pair<CellObjectsIterator, CellObjectsIterator>;
                
                std::vector<CellBeginEndPair> mCellBeginEndPairs;
                size_t mCurrentPairIndex = 0;
                
                Iterator(const std::vector<CellBeginEndPair>& vectorIteratorPairs) : mCellBeginEndPairs(vectorIteratorPairs) { }
                
                void moveToEnd() {
                    if (mCellBeginEndPairs.empty()) {
                        return;
                    }
                    
                    mCurrentPairIndex = mCellBeginEndPairs.size() - 1;
                    auto& end = mCellBeginEndPairs[mCurrentPairIndex].second;
                    mCellBeginEndPairs[mCurrentPairIndex] = std::make_pair(end, end);
                }
                
            public:
                Iterator& operator++() {
                    auto& pair = mCellBeginEndPairs[mCurrentPairIndex];
                    auto& current = pair.first;
                    auto& end = pair.second;
                    
                    if (current == end) {
                        if (mCurrentPairIndex == mCellBeginEndPairs.size() - 1) {
                            throw std::out_of_range("Incrementing an iterator which had reached the end already");
                        }
                        mCurrentPairIndex++;
                    } else {
                        current++;
                    }

                    return *this;
                }
                
                T& operator*() {
                    return *(mCellBeginEndPairs[mCurrentPairIndex].first);
                }
                
                T* operator->() {
                    return &(*(mCellBeginEndPairs[mCurrentPairIndex].first));
                }
                
                const T& operator*() const {
                    return *mCellBeginEndPairs[mCurrentPairIndex].first;
                }
                
                const T* operator->() const {
                    return &(*mCellBeginEndPairs[mCurrentPairIndex].first);
                }
                
                bool operator!=(const Iterator& other) const {
                    if (mCellBeginEndPairs.empty()) {
                        return mCurrentPairIndex != other.mCurrentPairIndex;
                    } else {
                        return mCurrentPairIndex != other.mCurrentPairIndex ||
                        mCellBeginEndPairs[mCurrentPairIndex].first != other.mCellBeginEndPairs[other.mCurrentPairIndex].first;
                    }
                }
            };
            
        private:
            friend SpatialHash;
            
            Iterator mBegin;
            Iterator mEnd;
            
            Range(const std::vector<typename Iterator::CellBeginEndPair>& cellBeginEndPairs)
            :
            mBegin(cellBeginEndPairs),
            mEnd(cellBeginEndPairs)
            {
                mEnd.moveToEnd();
            }
            
        public:
            Iterator begin() {
                return mBegin;
            }
            
            Iterator end() {
                return mEnd;
            }
        };

#pragma mark - SpatialHash's private contents
        
        std::unordered_map<Cell, Objects> mObjects;
        AxisAlignedBox3D mBoundaries;
        uint32_t mResolution;
        
        int32_t cellIndex(int32_t axis, float positionOnAxis) const {
            float delta = mBoundaries.max[axis] - mBoundaries.min[axis];
            if (fabs(delta) < 1e-09) {
                return 0;
            }
            return (positionOnAxis - mBoundaries.min[axis]) / delta * mResolution;
        }
        
        Cell cell(const glm::vec3& position) const {
            return std::make_tuple(cellIndex(0, position.x), cellIndex(1, position.y), cellIndex(2, position.z));
        }
        
        bool isCellValid(const Cell& cell) const {
            int32_t x = std::get<0>(cell);
            int32_t y = std::get<1>(cell);
            int32_t z = std::get<2>(cell);
            
            return (x >= 0 && x < mResolution) && (y >= 0 && y < mResolution) && (z >= 0 && z < mResolution);
        }
        
        std::vector<Cell> neighbours(const Cell& cell) const {
            std::vector<Cell> neighbours;
            
            for (int32_t x = -1; x <= 1; ++x) {
                for (int32_t y = -1; y <= 1; ++y) {
                    for (int32_t z = -1; z <= 1; ++z) {
                        Cell neighbour = std::make_tuple(std::get<0>(cell) + x, std::get<1>(cell) + y, std::get<2>(cell) + z);
                        if (isCellValid(neighbour) && mObjects.find(neighbour) != mObjects.end()) {
                            neighbours.push_back(neighbour);
                        }
                    }
                }
            }
            
            return neighbours;
        }
        
#pragma mark - SpatialHash's public contents
        
        public:
        
#pragma mark Lifecycle
        
        SpatialHash(const AxisAlignedBox3D& boundaries, uint32_t resolution)
        :
        mBoundaries(boundaries),
        mResolution(resolution)
        { }
        
#pragma mark Modifiers
        
        void insert(const T& object, const glm::vec3& position) {
            if (!mBoundaries.containsPoint(position)) {
                throw std::out_of_range("Attempt to insert an object outside of spatial hash's boundaries");
            }
            mObjects[cell(position)].push_back(object);
        }
        
        Range neighbours(const glm::vec3& position) {
            auto neighbourCells = neighbours(cell(position));
            std::vector<typename Range::Iterator::CellBeginEndPair> neighbourIteratorPairs;
            for (auto& cell : neighbourCells) {
                auto& objectsInCell = mObjects[cell];
                neighbourIteratorPairs.emplace_back(std::make_pair(objectsInCell.begin(), objectsInCell.end()));
            }

            return Range(neighbourIteratorPairs);
        }
        
#pragma mark Iteration
        
        ForwardIterator begin() {
            return ForwardIterator(mObjects.begin(), mObjects.end());
        }
        
        ForwardIterator end() {
            return ForwardIterator(mObjects.end(), mObjects.end());
        }
    };
    
    template<typename T>
    typename SpatialHash<T>::ForwardIterator begin(const SpatialHash<T>& hash) {
        return hash.begin();
    }
    
    template<typename T>
    typename SpatialHash<T>::ForwardIterator end(const SpatialHash<T>& hash) {
        return hash.end();
    }
    
}

#endif /* SpatialHash_hpp */
