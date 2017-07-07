//
//  AxesSystem.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 04.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "AxesSystem.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/vec3.hpp>

#include "Transformation.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    AxesSystem::AxesSystem()
    :
    mAxisBoundingBox({ -0.3, 0.0, -0.3 }, { 0.3, 3.0, 0.3 }),
    mXYSelectionRect({ 0.0, 0.0, 0.0 }, { SegmentHeight, 0.0, 0.0 }, { 0.0, SegmentHeight, 0.0 }),
    mXZSelectionRect({ 0.0, 0.0, 0.0 }, { SegmentHeight, 0.0, 0.0 }, { 0.0, 0.0, SegmentHeight }),
    mYZSelectionRect({ 0.0, 0.0, 0.0 }, { 0.0, SegmentHeight, 0.0 }, { 0.0, 0.0, SegmentHeight }),
    mAxisVertices {
        // Four bottom points of the arrow's base
        {{ -0.025, 0.0, 0.025 }}, // P0
        {{ -0.025, 0.0, -0.025 }}, // P1
        {{ 0.025, 0.0, -0.025 }}, // P2
        {{ 0.025, 0.0, 0.025 }}, // P3
        
        // Four top points of the arrow's base
        {{ -0.025, ArrowBaseHeight, 0.025 }}, // P4
        {{ -0.025, ArrowBaseHeight, -0.025 }}, // P5
        {{ 0.025, ArrowBaseHeight, -0.025 }}, // P6
        {{ 0.025, ArrowBaseHeight, 0.025 }}, // P7
        
        // Four bottom points of the arrow's head
        {{ -0.15, ArrowBaseHeight, 0.15 }}, // P8
        {{ -0.15, ArrowBaseHeight, -0.15 }}, // P9
        {{ 0.15, ArrowBaseHeight, -0.15 }}, // P10
        {{ 0.15, ArrowBaseHeight, 0.15 }}, // P11
        
        // Arrow head's top point
        {{ 0.0, 3.0, 0.0 }} // P12
    },
    mAxisVertexIndices {
        // Bottom quad
        0, 1, 2,
        0, 2, 3,
        
        // Side quad
        0, 7, 4,
        0, 3, 7,
        
        // Side quad
        3, 6, 7,
        3, 2, 6,
        
        // Side quad
        2, 5, 6,
        2, 1, 5,
        
        // Side quad
        1, 4, 5,
        1, 0, 4,
        
        // Head side triangles
        8, 12, 9,
        8, 11, 12,
        11, 10, 12,
        10, 9, 12,
        
        // Head bottom quad
        8, 10, 11,
        8, 9, 10
    },
    mSegmentVertices {
        // Four bottom points of the segment
        {{ -0.025, 0.0, 0.025 }}, // P0
        {{ -0.025, 0.0, -0.025 }}, // P1
        {{ 0.025, 0.0, -0.025 }}, // P2
        {{ 0.025, 0.0, 0.025 }}, // P3
        
        // Four top points of the segment
        {{ -0.025, SegmentHeight, 0.025 }}, // P4
        {{ -0.025, SegmentHeight, -0.025 }}, // P5
        {{ 0.025, SegmentHeight, -0.025 }}, // P6
        {{ 0.025, SegmentHeight, 0.025 }}, // P7
    },
    mSegmentVertexIndices {
        // Bottom quad
        0, 1, 2,
        0, 2, 3,
        
        // Side quad
        0, 7, 4,
        0, 3, 7,
        
        // Side quad
        3, 6, 7,
        3, 2, 6,
        
        // Side quad
        2, 5, 6,
        2, 1, 5,
        
        // Side quad
        1, 4, 5,
        1, 0, 4,
        
        // Top quad
        4, 6, 5,
        4, 7, 6
    }
    {
        GLVertexArrayLayoutDescription layout({ static_cast<int>(glm::vec3::length() * sizeof(GLfloat)) });
        mAxisVAO.initialize(mAxisVertices, 13, mAxisVertexIndices, 51, layout);
        mSegmentVAO.initialize(mSegmentVertices, 8, mSegmentVertexIndices, 36, layout);
        
        Transformation t;
        
        // XY
        t.translation = glm::vec3(SegmentHeight, 0.0, 0.0);
        mXYSegmentTransform = t.modelMatrix();
        
        // XZ
        t.translation = glm::vec3(SegmentHeight, 0.0, 0.0);
        t.rotation = glm::rotate(static_cast<float>(M_PI_2), glm::vec3(1.0, 0.0, 0.0));
        mXZSegmentTransform = t.modelMatrix();
        
        // YX
        t.translation = glm::vec3(SegmentHeight, SegmentHeight, 0.0);
        t.rotation = glm::rotate(static_cast<float>(M_PI_2), glm::vec3(0.0, 0.0, 1.0));
        mYXSegmentTransform = t.modelMatrix();
        
        // YZ
        t.translation = glm::vec3(0.0, SegmentHeight, 0.0);
        t.rotation = glm::rotate(static_cast<float>(M_PI_2), glm::vec3(1.0, 0.0, 0.0));
        mYZSegmentTransform = t.modelMatrix();
        
        // ZX
        t.translation = glm::vec3(SegmentHeight, 0.0, SegmentHeight);
        t.rotation = glm::rotate(static_cast<float>(M_PI_2), glm::vec3(0.0, 0.0, 1.0));
        mZXSegmentTransform = t.modelMatrix();
        
        // ZY
        t.translation = glm::vec3(0.0, 0.0, SegmentHeight);
        t.rotation = glm::quat();
        mZYSegmentTransform = t.modelMatrix();
    }
    
#pragma mark - Public
    
    glm::mat4 AxesSystem::rotationForAxis(Cartesian axis) const {
        switch (axis) {
            case CartesianAxis::x: { return glm::rotate(static_cast<float>(-M_PI_2), glm::vec3(0.0, 0.0, 1.0)); }
            case CartesianAxis::z: { return glm::rotate(static_cast<float>(M_PI_2), glm::vec3(1.0, 0.0, 0.0)); }
            default: { return glm::mat4(1.0); }
        }
    }
    
    const AxisAlignedBox3D& AxesSystem::axisBoundingBox() const {
        return mAxisBoundingBox;
    }
    
    const Parallelogram3D& AxesSystem::XYSelectionRect() const {
        return mXYSelectionRect;
    }
    
    const Parallelogram3D& AxesSystem::XZSelectionRect() const {
        return mXZSelectionRect;
    }
    
    const Parallelogram3D& AxesSystem::YZSelectionRect() const {
        return mYZSelectionRect;
    }
    
    const glm::mat4& AxesSystem::XYSegmentTransform() const {
        return mXYSegmentTransform;
    }
    
    const glm::mat4& AxesSystem::XZSegmentTransform() const {
        return mXZSegmentTransform;
    }
    
    const glm::mat4& AxesSystem::YXSegmentTransform() const {
        return mYXSegmentTransform;
    }
    
    const glm::mat4& AxesSystem::YZSegmentTransform() const {
        return mYZSegmentTransform;
    }
    
    const glm::mat4& AxesSystem::ZXSegmentTransform() const {
        return mZXSegmentTransform;
    }
    
    const glm::mat4& AxesSystem::ZYSegmentTransform() const {
        return mZYSegmentTransform;
    }
    
    void AxesSystem::drawAxis() const {
        mAxisVAO.bind();
        glDrawElements(GL_TRIANGLES, sizeof(mAxisVertexIndices) / sizeof(GLushort), GL_UNSIGNED_SHORT, nullptr);
    }
    
    void AxesSystem::drawSegment() const {
        mSegmentVAO.bind();
        glDrawElements(GL_TRIANGLES, sizeof(mSegmentVertexIndices) / sizeof(GLushort), GL_UNSIGNED_SHORT, nullptr);
    }
}
