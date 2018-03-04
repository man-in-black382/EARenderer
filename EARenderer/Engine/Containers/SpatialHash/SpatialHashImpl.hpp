//
//  SpatialHashImpl.h
//  EARenderer
//
//  Created by Pavlo Muratov on 04.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SpatialHashImpl_h
#define SpatialHashImpl_h

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
    int32_t
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
        return std::make_tuple(cellIndex(0, position.x), cellIndex(1, position.y), cellIndex(2, position.z));
    }

    template <typename T>
    bool
    SpatialHash<T>::isCellValid(const Cell& cell) const {
        int32_t x = std::get<0>(cell);
        int32_t y = std::get<1>(cell);
        int32_t z = std::get<2>(cell);

        return (x >= 0 && x < mResolution) && (y >= 0 && y < mResolution) && (z >= 0 && z < mResolution);
    }

    template <typename T>
    std::vector<typename SpatialHash<T>::Cell>
    SpatialHash<T>::neighbours(const Cell& cell) const {
        std::vector<Cell> neighbours;

        for (int32_t x = -1; x <= 1; ++x) {
            for (int32_t y = -1; y <= 1; ++y) {
                for (int32_t z = -1; z <= 1; ++z) {
                    Cell neighbour = std::make_tuple(std::get<0>(cell) + x, std::get<1>(cell) + y, std::get<2>(cell) + z);

                    bool valid = isCellValid(neighbour);
                    bool present = mObjects.find(neighbour) != mObjects.end();

                    if (valid && present) {
                        neighbours.push_back(neighbour);
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
        if (!mBoundaries.containsPoint(position)) {
            throw std::out_of_range("Attempt to insert an object outside of spatial hash's boundaries");
        }
        mObjects[cell(position)].push_back(object);
    }

    template <typename T>
    typename SpatialHash<T>::Range
    SpatialHash<T>::neighbours(const glm::vec3& position) {
        auto neighbourCells = neighbours(cell(position));
        std::vector<typename Range::CellBeginEndPair> neighbourIteratorPairs;
        for (auto& cell : neighbourCells) {
            auto& objectsInCell = mObjects[cell];
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
