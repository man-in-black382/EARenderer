//
//  Box.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 16.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "AxisAlignedBox3D.hpp"

#include <limits>
#include <glm/detail/func_geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec4.hpp>

namespace EARenderer {
    
#pragma mark - Static
    
    const AxisAlignedBox3D& AxisAlignedBox3D::zero() {
        static AxisAlignedBox3D b;
        return b;
    }
    
    const AxisAlignedBox3D& AxisAlignedBox3D::unit() {
        static AxisAlignedBox3D b(glm::vec3(1.0), glm::vec3(1.0));
        return b;
    }
    
    const AxisAlignedBox3D& AxisAlignedBox3D::maximumReversed() {
        auto min = glm::vec3(std::numeric_limits<float>::max());
        auto max = glm::vec3(std::numeric_limits<float>::lowest());
        static AxisAlignedBox3D b(min, max);
        return b;
    }
    
#pragma mark - Lifecycle
    
    AxisAlignedBox3D::AxisAlignedBox3D(const glm::vec3& min, const glm::vec3& max)
    :
    min(min),
    max(max)
    { }
    
#pragma mark - Math
    
    float AxisAlignedBox3D::diagonal() const {
        return glm::length(max - min);
    }
    
    glm::mat4 AxisAlignedBox3D::asFrustum() const {
        // Z component shenanigans due to NDC and world handedness inconsistency
        return glm::ortho(min.x, max.x, min.y, max.y, -max.z, -min.z);
    }
    
    std::array<glm::vec4, 8> AxisAlignedBox3D::cornerPoints() const {
        return std::array<glm::vec4, 8> {
            glm::vec4{ min.x, min.y, min.z, 1.f },
            glm::vec4{ min.x, max.y, min.z, 1.f },
            glm::vec4{ max.x, max.y, min.z, 1.f },
            glm::vec4{ max.x, min.y, min.z, 1.f },
            glm::vec4{ min.x, min.y, max.z, 1.f },
            glm::vec4{ min.x, max.y, max.z, 1.f },
            glm::vec4{ max.x, max.y, max.z, 1.f },
            glm::vec4{ max.x, min.y, max.z, 1.f }
        };
    }
    
    AxisAlignedBox3D AxisAlignedBox3D::transformedBy(const Transformation& t) const {
        return transformedBy(t.modelMatrix());
    }
    
    AxisAlignedBox3D AxisAlignedBox3D::transformedBy(const glm::mat4& m) const {
        glm::vec4 newMin = { min.x, min.y, min.z, 1.0 };
        glm::vec4 newMax = { max.x, max.y, max.z, 1.0 };
        newMin = m * newMin;
        newMax = m * newMax;
        newMin /= newMin.w;
        newMax /= newMax.w;
        return { glm::vec3(newMin), glm::vec3(newMax) };
    }
    
    bool AxisAlignedBox3D::containsPoint(const glm::vec3& point) {
        return point.x >= min.x && point.x <= max.x &&
        point.y >= min.y && point.y <= max.y &&
        point.z >= min.z && point.z <= max.z;
    }
    
}
