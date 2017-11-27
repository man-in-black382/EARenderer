//
//  Collision.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Collision.hpp"

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

namespace EARenderer {
    
#pragma mark - Private
    
    Interval Collision::GetInterval(const Triangle& triangle, const glm::vec3& axis) {
        Interval result;
        
        result.min = glm::dot(axis, triangle.points[0]);
        result.max = result.min;
        for (int i = 1; i < 3; ++i) {
            float value = glm::dot(axis, triangle.points[i]);
            result.min = fminf(result.min, value);
            result.max = fmaxf(result.max, value);
        }
        
        return result;
    }
    
    Interval Collision::GetInterval(const AxisAlignedBox3D& aabb, const glm::vec3& axis) {
        glm::vec3 i = aabb.min;
        glm::vec3 a = aabb.max;
        
        glm::vec3 vertex[8] = {
            glm::vec3(i.x, a.y, a.z),
            glm::vec3(i.x, a.y, i.z),
            glm::vec3(i.x, i.y, a.z),
            glm::vec3(i.x, i.y, i.z),
            glm::vec3(a.x, a.y, a.z),
            glm::vec3(a.x, a.y, i.z),
            glm::vec3(a.x, i.y, a.z),
            glm::vec3(a.x, i.y, i.z)
        };
        
        Interval result;
        result.min = result.max = glm::dot(axis, vertex[0]);
        
        for (int i = 1; i < 8; ++i) {
            float projection = glm::dot(axis, vertex[i]);
            result.min = (projection < result.min) ? projection : result.min;
            result.max = (projection > result.max) ? projection : result.max;
        }
        
        return result;
    }
    
    bool Collision::OverlapOnAxis(const AxisAlignedBox3D& aabb, const Triangle& triangle, const glm::vec3& axis) {
        Interval a = GetInterval(aabb, axis);
        Interval b = GetInterval(triangle, axis);
        return ((b.min <= a.max) && (a.min <= b.max));
    }
    
#pragma mark - Public
    
    bool Collision::TriangleAABB(const Triangle& t, const AxisAlignedBox3D& a) {
        // Compute the edge vectors of the triangle  (ABC)
        glm::vec3 f0 = t.b - t.a;
        glm::vec3 f1 = t.c - t.b;
        glm::vec3 f2 = t.a - t.c;
        
        // Compute the face normals of the AABB
        glm::vec3 u0(1.0f, 0.0f, 0.0f);
        glm::vec3 u1(0.0f, 1.0f, 0.0f);
        glm::vec3 u2(0.0f, 0.0f, 1.0f);
        
        glm::vec3 test[13] = {
            // 3 Normals of AABB
            u0, // AABB Axis 1
            u1, // AABB Axis 2
            u2, // AABB Axis 3
            // 1 Normal of the Triangle
            glm::cross(f0, f1),
            // 9 Axis, cross products of all edges
            glm::cross(u0, f0),
            glm::cross(u0, f1),
            glm::cross(u0, f2),
            glm::cross(u1, f0),
            glm::cross(u1, f1),
            glm::cross(u1, f2),
            glm::cross(u2, f0),
            glm::cross(u2, f1),
            glm::cross(u2, f2)
        };
        
        for (int i = 0; i < 13; ++i) {
            if (!OverlapOnAxis(a, t, test[i])) {
                return false; // Seperating axis found
            }
        }
        
        return true; // Seperating axis not found
    }
    
    bool Collision::RayAABB(const Ray3D& ray, const AxisAlignedBox3D& aabb, float& distance) {
        glm::vec3 inverseDirection = glm::vec3(1.0) / ray.direction;
        
        float t1 = (aabb.min.x - ray.origin.x) * inverseDirection.x;
        float t2 = (aabb.max.x - ray.origin.x) * inverseDirection.x;
        float t3 = (aabb.min.y - ray.origin.y) * inverseDirection.y;
        float t4 = (aabb.max.y - ray.origin.y) * inverseDirection.y;
        float t5 = (aabb.min.z - ray.origin.z) * inverseDirection.z;
        float t6 = (aabb.max.z - ray.origin.z) * inverseDirection.z;
        
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
    
    bool Collision::RayParallelogram(const Ray3D& ray, const Parallelogram3D& parallelogram, float& distance) {
        glm::vec3 normal = parallelogram.normal();
        float dirDotNorm = glm::dot(ray.direction, normal);
        if (fabs(dirDotNorm) < 0.001) { return false; }
        distance = glm::dot((parallelogram.corner - ray.origin), normal) / dirDotNorm;
        glm::vec3 intersectionPoint = ray.origin + distance * ray.direction;
        glm::vec3 cornerToIntersectionVec = intersectionPoint - parallelogram.corner;
        float projection1 = glm::dot(cornerToIntersectionVec, glm::normalize(parallelogram.side1));
        float projection2 = glm::dot(cornerToIntersectionVec, glm::normalize(parallelogram.side2));
        return (projection1 > 0 && projection1 < glm::length(parallelogram.side1) &&
                projection2 > 0 && projection2 < glm::length(parallelogram.side2));
    }
    
}
