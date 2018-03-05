//
//  SparseOctree.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 2/27/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SparseOctree_hpp
#define SparseOctree_hpp

#include <unordered_map>
#include <stack>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>

#include "AxisAlignedBox3D.hpp"
#include "Ray3D.hpp"

// Design and implementation of virtualized sparse voxel octree ray casting
// http://sam.cs.lth.se/ExjobGetFile?id=359

namespace EARenderer {

    template <typename T>
    class SparseOctree {
    private:

#pragma mark - Private nested types

        using NodeIndex = uint32_t;
        using BitMask = uint8_t;

        struct Node {
        private:
            // 8 MSB - child presence mask, 8 LSB - leaf mask
            uint16_t mChildInfo = 0;

        public:
            std::vector<T> objects;

            void setChildPresent(BitMask childIndex, bool isPresent);
            void setChildLeaf(BitMask childIndex, bool isLeaf);
            bool isChildPresent(BitMask childIndex) const;
            bool isChildLeaf(BitMask childIndex) const;
        };

        struct StackFrame {
            NodeIndex nodeIndex;
            uint8_t depth;
            float t_in;
            float t_out;
            glm::vec3 planesOffset;

            StackFrame(NodeIndex nodeIndex, uint8_t nodeDepth, float t_in, float t_out, const glm::vec3& planesOffset);
        };

        struct Iterator {

        };

        // In the original article authors use right-handed coordinate system with Z axis
        // pointing upwards. We swap Y and Z to match conventional OpenGL left-handed coordinate system
        // while preserving child indexing rules defined in the paper.
        // (Original bitmasks are: 100 for X, 010 for Y and 001 for Z)
        //
        static constexpr BitMask XBitMask = 0b100;
        static constexpr BitMask YBitMask = 0b001;
        static constexpr BitMask ZBitMask = 0b010;

        //   ORDER OF CHILDREN
        //
        //       +---+---+  Y
        //      / 4 / 0 /|  |
        //     /---/---/ |  +--X
        //    +---+---+|0|
        //    | 6 | 2 ||.+
        //    +---+---+'1|
        //    | 7 | 3 ||.+ <--Positive Z
        //    +---+---+' <--Negative Z
        //      ^--Behind: 5

#pragma mark - Private members

        size_t mMaximumDepth;
        AxisAlignedBox3D mBoundingBox;
        std::unordered_map<NodeIndex, Node> mNodes;
        std::stack<StackFrame> mTraversalStack;
        std::vector<float> mCuttingPlaneOffsets;

#pragma mark - Private functions

        glm::mat4 localNormalizedSpaceMatrix() const;

        NodeIndex appendChildIndex(NodeIndex parent, NodeIndex child);

        BitMask signMask(const glm::vec3& valueTriple) const;

        BitMask sortMaskByMinimum(const glm::vec3& valueTriple) const;

        BitMask sortMaskByMaximum(const glm::vec3& valueTriple) const;

        void pushRootNode();

        void pushChildNodes(const StackFrame& currentFrame,
                            const glm::vec3& t,
                            BitMask signMaskA,
                            BitMask signMaskB,
                            BitMask p_first,
                            BitMask p_last,
                            size_t planeIntersectionCounter);

    public:

#pragma mark - Public interface
#pragma mark - Lifecycle

        SparseOctree(const AxisAlignedBox3D& boundingBox, size_t maximumDepth);

#pragma mark - Traversal

        void raymarch(const Ray3D& ray);
    };

}

#include "SparseOctreeImpl.hpp"
#include "SparseOctreeNodeImpl.hpp"
#include "SparseOctreeIteratorImpl.hpp"
#include "SparseOctreeStackFrameImpl.hpp"

#endif /* SparseOctree_hpp */
