//
//  Ray.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Ray3D.hpp"

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include <algorithm>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    Ray3D::Ray3D(const glm::vec3& origin, const glm::vec3& end)
    :
    origin(origin),
    direction(end - origin)
    {
        direction = glm::normalize(direction);
    }
    
#pragma mark - Public
    
    bool Ray3D::intersectsAAB(const AxisAlignedBox3D& aab, float& distance) const {
        glm::vec3 inverseDirection = glm::vec3(1.0) / direction;
        
        float t1 = (aab.min.x - origin.x) * inverseDirection.x;
        float t2 = (aab.max.x - origin.x) * inverseDirection.x;
        float t3 = (aab.min.y - origin.y) * inverseDirection.y;
        float t4 = (aab.max.y - origin.y) * inverseDirection.y;
        float t5 = (aab.min.z - origin.z) * inverseDirection.z;
        float t6 = (aab.max.z - origin.z) * inverseDirection.z;
        
        float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
        float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));
        
        // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
        if (tmax < 0) {
            distance = tmax;
            return false;
        }
        
        // if tmin > tmax, ray doesn't intersect AABB
        if (tmin > tmax) {
            distance = tmax;
            return false;
        }
        
        distance = tmin;
        return true;
    }
    
    bool Ray3D::intersectsParallelogram(const Parallelogram3D& parallelogram, float& distance) const {
        glm::vec3 normal = parallelogram.normal();
        float dirDotNorm = glm::dot(direction, normal);
        if (fabs(dirDotNorm) < 0.001) { return false; }
        distance = glm::dot((parallelogram.corner - origin), normal) / dirDotNorm;
        glm::vec3 intersectionPoint = origin + distance * direction;
        glm::vec3 cornerToIntersectionVec = intersectionPoint - parallelogram.corner;
        float projection1 = glm::dot(cornerToIntersectionVec, glm::normalize(parallelogram.side1));
        float projection2 = glm::dot(cornerToIntersectionVec, glm::normalize(parallelogram.side2));
        return (projection1 > 0 && projection1 < glm::length(parallelogram.side1) &&
                projection2 > 0 && projection2 < glm::length(parallelogram.side2));
    }
    
    Ray3D Ray3D::transformedBy(const Transformation& t) const {
        return transformedBy(t.modelMatrix());
    }
    
    Ray3D Ray3D::transformedBy(const glm::mat4& m) const {
        glm::vec3 newOrigin = m * glm::vec4(origin, 1.0);
        glm::vec3 newDirection = m * glm::vec4(direction, 0.0);
        return Ray3D(newOrigin, newOrigin + newDirection);
    }
    
}
