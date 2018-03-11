//
//  SpatialHashImpl.h
//  EARenderer
//
//  Created by Pavlo Muratov on 04.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SpatialHashImpl_h
#define SpatialHashImpl_h

#include "Collision.hpp"

namespace EARenderer {

#pragma mark Lifecycle

    template <typename T>
    SpatialHash<T>::SpatialHash(const AxisAlignedBox3D& boundaries, uint32_t resolution)
    :
    mBoundaries(boundaries),
    mResolution(resolution)
    { }

#pragma mark - Accessors

    template <typename T>
    uint16_t
    SpatialHash<T>::cellIndex(int32_t axis, float positionOnAxis) const {
        float delta = mBoundaries.max[axis] - mBoundaries.min[axis];
        if (fabs(delta) < 1e-09) {
            return 0;
        }
        int32_t index = (positionOnAxis - mBoundaries.min[axis]) / delta * (mResolution - 1);
        return index;
    }

    template <typename T>
    typename SpatialHash<T>::Cell
    SpatialHash<T>::cell(const glm::vec3& position) const {
        Cell cell;
        cell.encodeX(cellIndex(0, position.x));
        cell.encodeY(cellIndex(1, position.y));
        cell.encodeZ(cellIndex(2, position.z));
        return cell;
    }

    template <typename T>
    bool
    SpatialHash<T>::isCellValid(uint16_t x, uint16_t y, uint16_t z) const {
        return (x >= 0 && x < mResolution) && (y >= 0 && y < mResolution) && (z >= 0 && z < mResolution);
    }

    template <typename T>
    std::vector<typename SpatialHash<T>::Cell>
    SpatialHash<T>::neighbours(const Cell& cell) const {
        std::vector<Cell> neighbours;

        uint16_t cx = cell.decodeX();
        uint16_t cy = cell.decodeY();
        uint16_t cz = cell.decodeZ();

        for (uint16_t x = -1; x <= 1; ++x) {
            for (uint16_t y = -1; y <= 1; ++y) {
                for (uint16_t z = -1; z <= 1; ++z) {
                    uint16_t newX = cx + x;
                    uint16_t newY = cy + y;
                    uint16_t newZ = cz + z;

                    bool valid = isCellValid(newX, newY, newZ);

                    if (valid) {
                        Cell neighbour(newX, newY, newZ);
                        bool present = mObjects.find(neighbour.hash()) != mObjects.end();
                        if (present) {
                            neighbours.push_back(neighbour);
                        }
                    }
                }
            }
        }

        return neighbours;
    }

#pragma mark - Modifiers

    template <typename T>
    void
    SpatialHash<T>::insert(const T& object, const glm::vec3& position) {
        if (!mBoundaries.contains(position)) {
            throw std::out_of_range("Attempt to insert an object outside of spatial hash's boundaries");
        }
        mObjects[cell(position).hash()].push_back(object);
    }

    template <typename T>
    typename SpatialHash<T>::Range
    SpatialHash<T>::neighbours(const glm::vec3& position) {
        auto neighbourCells = neighbours(cell(position));
        std::vector<typename Range::CellBeginEndPair> neighbourIteratorPairs;
        for (auto& cell : neighbourCells) {
            auto& objectsInCell = mObjects[cell.hash()];
            neighbourIteratorPairs.emplace_back(std::make_pair(objectsInCell.begin(), objectsInCell.end()));
        }
        return Range(neighbourIteratorPairs);
    }

#pragma mark Iteration

    template <typename T>
    typename SpatialHash<T>::ForwardIterator
    SpatialHash<T>::begin() {
        return ForwardIterator(mObjects.begin(), mObjects.end());
    }

    template <typename T>
    typename SpatialHash<T>::ForwardIterator
    SpatialHash<T>::end() {
        return ForwardIterator(mObjects.end(), mObjects.end());
    }

}

#endif /* SpatialHashImpl_h */
