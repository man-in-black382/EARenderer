//
//  Box.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 16.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Box.hpp"

#include <glm/detail/func_geometric.hpp>
#include <glm/vec4.hpp>

namespace EARenderer {
    
#pragma mark - Static
    
    const Box& Box::zero() {
        static Box b;
        return b;
    }
    
#pragma mark - Lifecycle
    
    Box::Box()
    :
    min(glm::zero<glm::vec3>()),
    max(glm::zero<glm::vec3>())
    { }
    
    Box::Box(const glm::vec3& min, const glm::vec3& max)
    :
    min(min),
    max(max)
    { }
    
#pragma mark - Swap
    
    void Box::swap(Box& that) {
        std::swap(min, that.min);
        std::swap(max, that.max);
    }
    
    void swap(Box& lhs, Box& rhs) {
        lhs.swap(rhs);
    }
    
#pragma mark - Math
    
    const float Box::diagonal() const {
        return glm::length(max - min);
    }
    
    Box Box::transformedBy(const Transformation& t) const {
        return transformedBy(t.modelMatrix());
    }
    
    Box Box::transformedBy(const glm::mat4& m) const {
        glm::vec4 newMin = { min.x, min.y, min.z, 1.0 };
        glm::vec4 newMax = { max.x, max.y, max.z, 1.0 };
        newMin = m * newMin;
        newMax = m * newMax;
        newMin /= newMin.w;
        newMax /= newMax.w;
        return { glm::vec3(newMin), glm::vec3(newMax) };
    }
    
}
