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

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>

#include "AxisAlignedBox3D.hpp"
#include "Ray3D.hpp"

namespace EARenderer {

//    template <typename T>
    class SparseOctree {
    private:
        using NodeIndex = uint64_t;
        using BitMask = uint8_t;

        struct Node {
            // 15 - child pointer, 1 - far pointer flag, 8 - validity mask, 8 - leaf mask
            uint32_t mFlagSet;
        };

        struct TraversalStackFrame {
            NodeIndex nodeIndex;
            uint8_t depth;
            float t_in;
            float t_out;

            TraversalStackFrame(NodeIndex nodeIndex, uint8_t nodeDepth, float t_in, float t_out)
            :
            nodeIndex(nodeIndex),
            depth(nodeDepth),
            t_in(t_in),
            t_out(t_out)
            { }
        };

        static constexpr BitMask XBitMask = 0b100;
        static constexpr BitMask YBitMask = 0b010;
        static constexpr BitMask ZBitMask = 0b001;

        size_t mMaximumDepth;
        AxisAlignedBox3D mBoundingBox;
        std::unordered_map<NodeIndex, Node> mNodes;
        std::stack<TraversalStackFrame> mTraversalStack;

        glm::mat4 localNormalizedSpaceMatrix() const {
            glm::mat4 translation = glm::translate(-mBoundingBox.center());
            glm::vec3 bbAxisLengths = mBoundingBox.max - mBoundingBox.min;
            glm::mat4 scale = glm::scale(2.f / bbAxisLengths);
            return scale + translation;
        }

        BitMask signMask(const glm::vec3& valueTriple) const {
            BitMask mask = 0;
            if (valueTriple.x < 0.0) { mask |= XBitMask; }
            if (valueTriple.y < 0.0) { mask |= YBitMask; }
            if (valueTriple.z < 0.0) { mask |= ZBitMask; }
            return mask;
        }

    public:
        SparseOctree(const AxisAlignedBox3D& boundingBox, size_t maximumDepth)
        :
        mBoundingBox(boundingBox),
        mMaximumDepth(maximumDepth)
        { }

        void raymarch(const Ray3D& ray) {
            glm::mat4 localSpaceMat = localNormalizedSpaceMatrix();

            // Intersection points
            float t0 = 0.f;
            float t1 = 1.f;

            glm::vec3 a = ray.origin + t0 * ray.direction;
            glm::vec3 b = ray.origin + t1 * ray.direction;

            // Transform to voxelspace
            a = localSpaceMat * glm::vec4(a, 1.0);
            b = localSpaceMat * glm::vec4(b, 1.0);

            glm::vec3 ab = b - a;

            mTraversalStack.push(TraversalStackFrame(0, 0, 0.f, 1.f));

            while (!mTraversalStack.empty()) {
                auto stackFrame = mTraversalStack.top();
                mTraversalStack.pop();

                glm::vec3 p_in = (1.f - stackFrame.t_in) * a + stackFrame.t_out * b;
                glm::vec3 p_out = (1.f - stackFrame.t_out) * a + stackFrame.t_out * b;

                BitMask signMaskA = signMask(p_in + glm::vec3(stackFrame.depth));
                BitMask signMaskB = signMask(p_out + glm::vec3(stackFrame.depth));
                BitMask signMaskC = signMaskA ^ signMaskB;

                glm::vec3 t(std::numeric_limits<float>::max());
                size_t planeIntersectionCounter = 0;

//                BitMask p_first =

                if (signMaskC & XBitMask) {
                    t.x = ((float)stackFrame.depth - a.x) / ab.x;
                    planeIntersectionCounter++;
                }

                if (signMaskC & YBitMask) {
                    t.y = ((float)stackFrame.depth - a.y) / ab.y;
                    planeIntersectionCounter++;
                }

                if (signMaskC & ZBitMask) {
                    t.z = ((float)stackFrame.depth - a.z) / ab.z;
                    planeIntersectionCounter++;
                }
            }
        }

    };

}

#endif /* SparseOctree_hpp */
