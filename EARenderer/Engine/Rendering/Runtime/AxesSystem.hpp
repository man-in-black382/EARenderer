//
//  AxesSystem.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 04.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef AxesSystem_hpp
#define AxesSystem_hpp

#include "Vertex1P3.hpp"
#include "GLVertexArray.hpp"

#include "AxisAlignedBox3D.hpp"
#include "Parallelogram3D.hpp"
#include "Transformation.hpp"
#include "Camera.hpp"
#include "BitwiseEnum.hpp"

#include <array>
#include <glm/mat4x4.hpp>

namespace EARenderer {

    class AxesSystem {
    public:
        enum class Cartesian : int8_t {
            x = 1 << 0,
            y = 1 << 1,
            z = 1 << 2,
            none = 0
        };

    private:
        float ArrowBaseHeight = 2.0;
        float SegmentHeight = ArrowBaseHeight / 2.0;

        Vertex1P3 mAxisVertices[13];
        GLushort mAxisVertexIndices[51];

        Vertex1P3 mSegmentVertices[8];
        GLushort mSegmentVertexIndices[36];

        AxisAlignedBox3D mAxisBoundingBox;
        Parallelogram3D mXYSelectionRect;
        Parallelogram3D mXZSelectionRect;
        Parallelogram3D mYZSelectionRect;

        glm::mat4 mXYSegmentTransform;
        glm::mat4 mXZSegmentTransform;
        glm::mat4 mYXSegmentTransform;
        glm::mat4 mYZSegmentTransform;
        glm::mat4 mZXSegmentTransform;
        glm::mat4 mZYSegmentTransform;

//        GLVertexArray<Vertex1P3> mAxisVAO;
//        GLVertexArray<Vertex1P3> mSegmentVAO;

    public:
        AxesSystem();

        const AxisAlignedBox3D &axisBoundingBox() const;

        const Parallelogram3D &XYSelectionRect() const;

        const Parallelogram3D &XZSelectionRect() const;

        const Parallelogram3D &YZSelectionRect() const;

        const glm::mat4 &XYSegmentTransform() const;

        const glm::mat4 &XZSegmentTransform() const;

        const glm::mat4 &YXSegmentTransform() const;

        const glm::mat4 &YZSegmentTransform() const;

        const glm::mat4 &ZXSegmentTransform() const;

        const glm::mat4 &ZYSegmentTransform() const;

        std::array<Cartesian, 3> allAxes() const;

        glm::mat4 rotationForAxis(Cartesian axis) const;

        glm::mat4 worldTransformation(Transformation targetTransform, const glm::vec3 &anchorPoint) const;

        glm::vec3 basisVectorForAxis(Cartesian axis) const;

        void drawAxis() const;

        void drawSegment() const;
    };

    using CartesianAxis = AxesSystem::Cartesian;

}

ENABLE_BITMASK_OPERATORS(EARenderer::AxesSystem::Cartesian);

#endif /* AxesSystem_hpp */
