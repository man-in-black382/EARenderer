//
//  SparseOctreeImpl.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 04.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SparseOctreeImpl_h
#define SparseOctreeImpl_h

#include <stdexcept>

#include "StringUtils.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    template <typename T>
    SparseOctree<T>::SparseOctree(const AxisAlignedBox3D& boundingBox,
                                  size_t maximumDepth,
                                  const ContainmentDetector& containmentDetector,
                                  const CollisionDetector& collisionDetector)
    :
    mBoundingBox(boundingBox),
    mMaximumDepth(maximumDepth),
    mContainmentDetector(containmentDetector),
    mCollisionDetector(collisionDetector)
    {
        if (maximumDepth > mDepthCap) {
            throw std::invalid_argument(string_format("Octree maximum depth is %d, you requested %d", mDepthCap, maximumDepth));
        }

        // Root node shoud have 0 offset
        mCuttingPlaneOffsets.emplace_back(0.0);
        for (size_t depth = 1; depth <= maximumDepth; depth++) {
            mCuttingPlaneOffsets.emplace_back(1.0 / std::pow(2, depth));
        }
    }

#pragma mark - Internal logic

    template <typename T>
    glm::mat4
    SparseOctree<T>::localNormalizedSpaceMatrix() const {
        glm::mat4 translation = glm::translate(-mBoundingBox.center());
        glm::vec3 bbAxisLengths = mBoundingBox.max - mBoundingBox.min;
        glm::mat4 scale = glm::scale(2.f / bbAxisLengths);
        return scale * translation;
    }

    template <typename T>
    void
    SparseOctree<T>::pushRootNode() {
        NodeIndex rootNodeIndex = RootNodeIndex;
        uint8_t rootDepth = 0;
        float rootT1 = 0.0;
        float rootT2 = 1.0;

        mTraversalStack.push(StackFrame(rootNodeIndex, rootDepth, rootT1, rootT2));
    }

    template <typename T>
    typename SparseOctree<T>::NodeIndex
    SparseOctree<T>::appendChildIndex(NodeIndex parent, NodeIndex child) {
        parent <<= 3;
        parent |= child;
        return parent;
    }

    template <typename T>
    typename SparseOctree<T>::BitMask
    SparseOctree<T>::signMask(const glm::vec3& valueTriple) const {
        BitMask mask = 0;

        if (valueTriple.x < 0.0) { mask |= XBitMask; }
        if (valueTriple.y < 0.0) { mask |= YBitMask; }
        if (valueTriple.z < 0.0) { mask |= ZBitMask; }

        return mask;
    }

    template <typename T>
    typename SparseOctree<T>::BitMask
    SparseOctree<T>::sortMaskByMinimum(const glm::vec3& valueTriple) const {
        BitMask mask = 0;

        float minimum = std::min(valueTriple.x, valueTriple.y);
        minimum = std::min(minimum, valueTriple.z);

        if (valueTriple.x == minimum) { mask |= XBitMask; }
        if (valueTriple.y == minimum) { mask |= YBitMask; }
        if (valueTriple.z == minimum) { mask |= ZBitMask; }

        return mask;
    }

    template <typename T>
    typename SparseOctree<T>::BitMask
    SparseOctree<T>::sortMaskByMaximum(const glm::vec3& valueTriple) const {
        BitMask mask = 0;

        float maximum = std::max(valueTriple.x, valueTriple.y);
        maximum = std::max(maximum, valueTriple.z);

        if (valueTriple.x == maximum) { mask |= XBitMask; }
        if (valueTriple.y == maximum) { mask |= YBitMask; }
        if (valueTriple.z == maximum) { mask |= ZBitMask; }

        return mask;
    }

    template <typename T>
    void
    SparseOctree<T>::pushChildNodes(const StackFrame& currentFrame,
                                    const glm::vec3& t,
                                    BitMask signMaskA,
                                    BitMask signMaskB,
                                    BitMask p_first,
                                    BitMask p_last,
                                    size_t planeIntersectionCounter)
    {
        NodeIndex parentIndex = currentFrame.nodeIndex;
        NodeIndex childIndex = 0;

        if (planeIntersectionCounter == 3) {
            // TODO: check for node presense

            childIndex = appendChildIndex(parentIndex, signMaskB);
            mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, t.x, currentFrame.t_out));

            childIndex = appendChildIndex(parentIndex, signMaskB ^ p_last);
            if (mTraversalStack.top().nodeIndex != childIndex) {
                mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, t.y, t.x));
            }

            childIndex = appendChildIndex(parentIndex, signMaskA ^ p_first);
            if (mTraversalStack.top().nodeIndex != childIndex) {
                mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, t.z, t.y));
            }

            childIndex = appendChildIndex(parentIndex, signMaskA);
            if (mTraversalStack.top().nodeIndex != childIndex) {
                mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, currentFrame.t_in, t.z));
            }

        } else if (planeIntersectionCounter == 2) {

            childIndex = appendChildIndex(parentIndex, signMaskB);
            mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, t.y, currentFrame.t_out));

            childIndex = appendChildIndex(parentIndex, signMaskA ^ p_first);
            if (mTraversalStack.top().nodeIndex != childIndex) {
                mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, t.z, t.y));
            }

            childIndex = appendChildIndex(parentIndex, signMaskA);
            if (mTraversalStack.top().nodeIndex != childIndex) {
                mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, currentFrame.t_in, t.z));
            }

        } else if (planeIntersectionCounter == 1) {

            childIndex = appendChildIndex(parentIndex, signMaskB);
            mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, t.z, currentFrame.t_out));

            childIndex = appendChildIndex(parentIndex, signMaskA);
            mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, currentFrame.t_in, t.z));

        } else {

            childIndex = appendChildIndex(parentIndex, signMaskA);
            mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, currentFrame.t_in, currentFrame.t_out));

        }
    }

#pragma mark - Building

    template <typename T>
    void
    SparseOctree<T>::insert(const T& object) {
        std::unordered_map<uint8_t, BitMask> childBoxChildNodeCorrespondenceMap({
            std::make_pair(0, 7),
            std::make_pair(1, 3),
            std::make_pair(2, 6),
            std::make_pair(3, 2),
            std::make_pair(4, 5),
            std::make_pair(5, 1),
            std::make_pair(6, 4),
            std::make_pair(7, 0),
        });

        StackFrame stackFrame(RootNodeIndex, 0);
        AxisAlignedBox3D nodeBoundingBox = mBoundingBox;

//        printf("Inserting object in the octree\n");

        while (true) {
//            printf("Traversing through node %d\n", stackFrame.nodeIndex);

            Node& node = mNodes[stackFrame.nodeIndex];
            node.mBoundingBox = nodeBoundingBox;
            std::array<AxisAlignedBox3D, 8> childBoxes = node.mBoundingBox.octet();

            bool anyChildContainsObject = false;
            BitMask childNodeMask = 0;

            for (size_t i = 0; i < 8; i++) {
                bool boxContainsObject = mContainmentDetector(object, childBoxes[i]);
                if (boxContainsObject) {
                    childNodeMask = childBoxChildNodeCorrespondenceMap[i];
                    nodeBoundingBox = childBoxes[i];
                    node.setChildPresent(childNodeMask, true);
                    NodeIndex childIndex = appendChildIndex(stackFrame.nodeIndex, childNodeMask);
                    stackFrame = StackFrame(childIndex, stackFrame.depth + 1);
                    anyChildContainsObject = true;
                    break;
                }
            }

            if (!anyChildContainsObject || stackFrame.depth >= mMaximumDepth) {
                node.mObjects.emplace_back(object);
                break;
            }
        }
    }

#pragma mark - Traversal

    template <typename T>
    void
    SparseOctree<T>::raymarch(const glm::vec3& p0, const glm::vec3& p1) {
        glm::mat4 localSpaceMat = localNormalizedSpaceMatrix();

        // Transform to voxelspace
        glm::vec3 a = localSpaceMat * glm::vec4(p0, 1.0);
        glm::vec3 b = localSpaceMat * glm::vec4(p1, 1.0);

        glm::vec3 ab = b - a;

        glm::vec3 invA = 1.f / a;
        glm::vec3 a_ab = a / ab;

        pushRootNode();

        while (!mTraversalStack.empty()) {
            StackFrame stackFrame = mTraversalStack.top();
            mTraversalStack.pop();

            printf("Index %d\n", stackFrame.nodeIndex);

            if (stackFrame.depth >= mMaximumDepth) {
                continue;
            }

            glm::vec3 p_in = (1.f - stackFrame.t_in) * a + stackFrame.t_in * b;
            glm::vec3 p_out = (1.f - stackFrame.t_out) * a + stackFrame.t_out * b;

            glm::vec3 d;
            d.x = stackFrame.nodeIndex & XBitMask ? -mCuttingPlaneOffsets[stackFrame.depth] : mCuttingPlaneOffsets[stackFrame.depth];
            d.y = stackFrame.nodeIndex & YBitMask ? -mCuttingPlaneOffsets[stackFrame.depth] : mCuttingPlaneOffsets[stackFrame.depth];
            d.z = stackFrame.nodeIndex & ZBitMask ? -mCuttingPlaneOffsets[stackFrame.depth] : mCuttingPlaneOffsets[stackFrame.depth];

            BitMask signMaskA = signMask(p_in + d);
            BitMask signMaskB = signMask(p_out + d);
            BitMask signMaskC = signMaskA ^ signMaskB;

            glm::vec3 t(std::numeric_limits<float>::max());
            size_t planeIntersectionCounter = 0;

            if (signMaskC & XBitMask) {
                t.x = d.x * invA.x - a_ab.x;
                planeIntersectionCounter++;
            }

            if (signMaskC & YBitMask) {
                t.y = d.y * invA.y - a_ab.y;
                planeIntersectionCounter++;
            }

            if (signMaskC & ZBitMask) {
                t.z = d.z * invA.z - a_ab.z;
                planeIntersectionCounter++;
            }

            BitMask p_first = sortMaskByMinimum(t);
            BitMask p_last = sortMaskByMaximum(t);

            pushChildNodes(stackFrame, t, signMaskA, signMaskB, p_first, p_last, planeIntersectionCounter);
        }
    }

    template <typename T>
    void
    SparseOctree<T>::raymarch(const Ray3D& ray) {
        // Extend ray's end point to ensure that it reaches all across the bounding box
        float t1 = mBoundingBox.diagonal();
        glm::vec3 a = ray.origin;
        glm::vec3 b = ray.origin + ray.direction * t1;

        raymarch(a, b);
    }

#pragma mark Iteration

    template <typename T>
    typename SparseOctree<T>::Iterator
    SparseOctree<T>::begin() {
        return Iterator(mNodes.begin(), mNodes.end());
    }

    template <typename T>
    typename SparseOctree<T>::Iterator
    SparseOctree<T>::end() {
        return Iterator(mNodes.end());
    }

}

#endif /* SparseOctreeImpl_h */
