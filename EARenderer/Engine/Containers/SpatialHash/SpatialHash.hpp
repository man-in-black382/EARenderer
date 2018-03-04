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
            
            ForwardIterator(MapIterator i, MapIterator endIterator);
            
        public:
            ForwardIterator& operator++();

            T& operator*();
            
            T* operator->();
            
            const T& operator*() const;
            
            const T* operator->() const;

            bool operator!=(const ForwardIterator& other) const;
        };
        
#pragma mark Range iterator

        class Range {
        private:
            using CellObjectsIterator = typename std::vector<T>::iterator;
            using CellBeginEndPair = std::pair<CellObjectsIterator, CellObjectsIterator>;
            
        public:
            class Iterator {
            private:
                friend SpatialHash;
                friend Range;
                
                std::vector<CellBeginEndPair> mCellBeginEndPairs;
                size_t mCurrentPairIndex = 0;
                
                Iterator(const std::vector<CellBeginEndPair>& vectorIteratorPairs);
                
                void moveToEnd();
                
            public:
                Iterator& operator++();
                
                T& operator*();
                
                T* operator->();
                
                const T& operator*() const;
            
                const T* operator->() const;
                
                bool operator!=(const Iterator& other) const;
            };
            
        private:
            friend SpatialHash;
            
            Iterator mBegin;
            Iterator mEnd;
            
            Range(const std::vector<CellBeginEndPair>& cellBeginEndPairs);
            
        public:
            Iterator begin();

            Iterator end();
        };

#pragma mark - SpatialHash's private contents
        
        std::unordered_map<Cell, Objects> mObjects;
        AxisAlignedBox3D mBoundaries;
        uint32_t mResolution;
        
        int32_t cellIndex(int32_t axis, float positionOnAxis) const;
        
        Cell cell(const glm::vec3& position) const;
        
        bool isCellValid(const Cell& cell) const;
        
        std::vector<Cell> neighbours(const Cell& cell) const;
        
#pragma mark - SpatialHash's public contents
#pragma mark Lifecycle

    public:
        SpatialHash(const AxisAlignedBox3D& boundaries, uint32_t resolution);
        
#pragma mark Modifiers
        
        void insert(const T& object, const glm::vec3& position);
        
        Range neighbours(const glm::vec3& position);
        
#pragma mark Iteration
        
        ForwardIterator begin();
        
        ForwardIterator end();
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

#include "SpatialHashImpl.hpp"
#include "SpatialHashForwardIteratorImpl.hpp"
#include "SpatialHashRangeImpl.hpp"
#include "SpatialHashRangeIteratorImpl.hpp"

#endif /* SpatialHash_hpp */
