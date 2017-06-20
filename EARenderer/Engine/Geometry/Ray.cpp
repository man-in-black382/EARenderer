//
//  Ray.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Ray.hpp"

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>

#include <algorithm>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    Ray::Ray(const glm::vec3& origin, const glm::vec3& end)
    :
    mOrigin(origin),
    mDirection(end - origin)
    {
        mDirection = glm::normalize(mDirection);
    }
    
#pragma mark - Getters
    
    const glm::vec3& Ray::origin() const {
        return mOrigin;
    }
    
    const glm::vec3& Ray::direction() const {
        return mDirection;
    }
    
#pragma mark - Public
    
    bool Ray::intersectsAABB(const Box& aabb, float *distance) {
        glm::vec3 inverseDirection = glm::vec3(1.0) / mDirection;
        
        float t1 = (aabb.min.x - mOrigin.x) * inverseDirection.x;
        float t2 = (aabb.max.x - mOrigin.x) * inverseDirection.x;
        float t3 = (aabb.min.y - mOrigin.y) * inverseDirection.y;
        float t4 = (aabb.max.y - mOrigin.y) * inverseDirection.y;
        float t5 = (aabb.min.z - mOrigin.z) * inverseDirection.z;
        float t6 = (aabb.max.z - mOrigin.z) * inverseDirection.z;
        
        float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
        float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));
        
        // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
        if (tmax < 0) {
            if (distance) { *distance = tmax; }
            return false;
        }
        
        // if tmin > tmax, ray doesn't intersect AABB
        if (tmin > tmax) {
            if (distance) { *distance = tmax; }
            return false;
        }
        
        if (distance) { *distance = tmin; }
        return true;
    }
    
    Ray Ray::transformedBy(const Transformation& t) const {
        return transformedBy(t.modelMatrix());
    }
    
    Ray Ray::transformedBy(const glm::mat4& m) const {
        glm::vec3 origin = glm::vec3(m * glm::vec4(mOrigin, 1.0));
        glm::vec3 direction = glm::vec3(m * glm::vec4(mDirection, 0.0));
        return Ray(origin, origin + direction);
    }
    
}
