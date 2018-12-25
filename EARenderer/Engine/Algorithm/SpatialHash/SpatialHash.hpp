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

    template<class T>
    class SpatialHash {
    private:

#pragma mark - Type aliases

        using Objects = std::vector<T>;

#pragma mark - Nested types
#pragma mark Cell

        struct Cell {
        private:
            uint64_t mHash = 0;

        public:
            Cell() = default;

            Cell(uint16_t x, uint16_t y, uint16_t z);

            static Cell InvalidCell();

            uint64_t hash() const;

            void encodeX(uint16_t x);

            void encodeY(uint16_t x);

            void encodeZ(uint16_t x);

            uint16_t decodeX() const;

            uint16_t decodeY() const;

            uint16_t decodeZ() const;

            bool operator==(Cell that) const;

            bool operator!=(Cell that) const;
        };

#pragma mark Forward iterator

        class ForwardIterator {
        private:
            friend SpatialHash;

            using MapIterator = typename std::unordered_map<uint64_t, Objects>::iterator;
            using VectorIterator = typename std::vector<T>::iterator;

            MapIterator mMapIterator;
            MapIterator mMapEndIterator;
            VectorIterator mCurrentVectorIterator;

            ForwardIterator(MapIterator i, MapIterator endIterator);

        public:
            ForwardIterator &operator++();

            T &operator*();

            T *operator->();

            const T &operator*() const;

            const T *operator->() const;

            bool operator!=(const ForwardIterator &other) const;
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

                std::array<CellBeginEndPair, 27> mCellBeginEndPairs;
                size_t mCurrentPairIndex = 0;
                size_t mPairsCount;

                Iterator(const std::array<CellBeginEndPair, 27> &vectorIteratorPairs, size_t pairsCount);

                void moveToEnd();

            public:
                Iterator &operator++();

                T &operator*();

                T *operator->();

                const T &operator*() const;

                const T *operator->() const;

                bool operator!=(const Iterator &other) const;
            };

        private:
            friend SpatialHash;

            Iterator mBegin;
            Iterator mEnd;

            Range(const std::array<CellBeginEndPair, 27> &cellBeginEndPairs, size_t pairsCount);

        public:
            Iterator begin();

            Iterator end();
        };

#pragma mark - SpatialHash's private contents

        std::unordered_map<uint64_t, Objects> mObjects;
        AxisAlignedBox3D mBoundaries;
        uint16_t mResolution;
        size_t mSize;

        uint16_t cellIndex(int32_t axis, float positionOnAxis) const;

        Cell cell(const glm::vec3 &position) const;

        bool isCellValid(uint16_t x, uint16_t y, uint16_t z) const;

        std::array<Cell, 27> neighbours(const Cell &cell) const;

#pragma mark - SpatialHash's public contents
#pragma mark Lifecycle

    public:
        SpatialHash(const AxisAlignedBox3D &boundaries, uint32_t resolution);

#pragma mark Modifiers

        void insert(const T &object, const glm::vec3 &position);

        void erase(const ForwardIterator &it);

        Range neighbours(const glm::vec3 &position);

        size_t size() const;

#pragma mark Iteration

        ForwardIterator begin();

        ForwardIterator end();
    };

    template<typename T>
    typename SpatialHash<T>::ForwardIterator begin(const SpatialHash<T> &hash) {
        return hash.begin();
    }

    template<typename T>
    typename SpatialHash<T>::ForwardIterator end(const SpatialHash<T> &hash) {
        return hash.end();
    }

}

#include "SpatialHashImpl.hpp"
#include "SpatialHashCellImpl.h"
#include "SpatialHashForwardIteratorImpl.hpp"
#include "SpatialHashRangeImpl.hpp"
#include "SpatialHashRangeIteratorImpl.hpp"

#endif /* SpatialHash_hpp */
