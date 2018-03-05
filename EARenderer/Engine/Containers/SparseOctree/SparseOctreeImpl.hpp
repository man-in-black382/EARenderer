//
//  SparseOctreeImpl.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 04.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SparseOctreeImpl_h
#define SparseOctreeImpl_h

namespace EARenderer {

#pragma mark - Lifecycle

    template <typename T>
    SparseOctree<T>::SparseOctree(const AxisAlignedBox3D& boundingBox, size_t maximumDepth)
    :
    mBoundingBox(boundingBox),
    mMaximumDepth(maximumDepth)
    {
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
        NodeIndex rootNodeIndex = 0b1;
        uint8_t rootDepth = 0;
        float rootT1 = 0.0;
        float rootT2 = 1.0;
        glm::vec3 planesOffset(0.0);

        mTraversalStack.push(StackFrame(rootNodeIndex, rootDepth, rootT1, rootT2, planesOffset));
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
            mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, t.x, currentFrame.t_out, currentFrame.planesOffset));

            childIndex = appendChildIndex(parentIndex, signMaskB ^ p_last);
            mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, t.y, t.x, currentFrame.planesOffset));

            childIndex = appendChildIndex(parentIndex, signMaskA ^ p_first);
            mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, t.z, t.y, currentFrame.planesOffset));

            childIndex = appendChildIndex(parentIndex, signMaskA);
            mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, currentFrame.t_in, t.z, currentFrame.planesOffset));

        } else if (planeIntersectionCounter == 2) {

            childIndex = appendChildIndex(parentIndex, signMaskB);
            mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, t.y, currentFrame.t_out, currentFrame.planesOffset));

            childIndex = appendChildIndex(parentIndex, signMaskA ^ p_first);
            mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, t.z, t.y, currentFrame.planesOffset));

            childIndex = appendChildIndex(parentIndex, signMaskA);
            mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, currentFrame.t_in, t.z, currentFrame.planesOffset));

        } else if (planeIntersectionCounter == 1) {

            childIndex = appendChildIndex(parentIndex, signMaskB);
            mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, t.z, currentFrame.t_out, currentFrame.planesOffset));

            childIndex = appendChildIndex(parentIndex, signMaskA);
            mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, currentFrame.t_in, t.z, currentFrame.planesOffset));

        } else {

            childIndex = appendChildIndex(parentIndex, signMaskA);
            mTraversalStack.push(StackFrame(childIndex, currentFrame.depth + 1, currentFrame.t_in, currentFrame.t_out, currentFrame.planesOffset));

        }
    }

    template <typename T>
    void
    SparseOctree<T>::raymarch(const Ray3D& ray) {
        glm::mat4 localSpaceMat = localNormalizedSpaceMatrix();

        // Intersection points
        float t0 = 0.f;
        float t1 = 1.0;//1.7677;//1.f;

        //            glm::vec3 a = ray.origin + t0 * ray.direction;
        //            glm::vec3 b = ray.origin + t1 * ray.direction;

        glm::vec3 a { -0.1, -1, -1 };
        glm::vec3 b { 0.5, 0.3, 1 };

        // Transform to voxelspace
        a = localSpaceMat * glm::vec4(a, 1.0);
        b = localSpaceMat * glm::vec4(b, 1.0);

        glm::vec3 ab = b - a;

        pushRootNode();

        while (!mTraversalStack.empty()) {
            auto stackFrame = mTraversalStack.top();
            mTraversalStack.pop();

            float depth = stackFrame.depth;
            glm::vec3 p_in = (1.f - stackFrame.t_in) * a + stackFrame.t_in * b;
            glm::vec3 p_out = (1.f - stackFrame.t_out) * a + stackFrame.t_out * b;

            glm::vec3 d;
            d.x = stackFrame.nodeIndex & XBitMask ? -mCuttingPlaneOffsets[stackFrame.depth] : mCuttingPlaneOffsets[stackFrame.depth];
            d.y = stackFrame.nodeIndex & YBitMask ? -mCuttingPlaneOffsets[stackFrame.depth] : mCuttingPlaneOffsets[stackFrame.depth];
            d.z = stackFrame.nodeIndex & ZBitMask ? -mCuttingPlaneOffsets[stackFrame.depth] : mCuttingPlaneOffsets[stackFrame.depth];

            if (depth >= mMaximumDepth) {
                continue;
            }

            BitMask signMaskA = signMask(p_in + d);
            BitMask signMaskB = signMask(p_out + d);
            BitMask signMaskC = signMaskA ^ signMaskB;

            glm::vec3 t(std::numeric_limits<float>::max());
            size_t planeIntersectionCounter = 0;

            if (signMaskC & XBitMask) {
                t.x = (d.x - a.x) / ab.x;
                planeIntersectionCounter++;
            }

            if (signMaskC & YBitMask) {
                t.y = (d.y - a.y) / ab.y;
                planeIntersectionCounter++;
            }

            if (signMaskC & ZBitMask) {
                t.z = (d.z - a.z) / ab.z;
                planeIntersectionCounter++;
            }

            BitMask p_first = sortMaskByMinimum(t);
            BitMask p_last = sortMaskByMaximum(t);

            pushChildNodes(stackFrame, t, signMaskA, signMaskB, p_first, p_last, planeIntersectionCounter);
        }
    }

}

#endif /* SparseOctreeImpl_h */
