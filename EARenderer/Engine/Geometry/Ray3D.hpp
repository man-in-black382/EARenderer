//
//  Ray.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Ray_hpp
#define Ray_hpp

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "AxisAlignedBox3D.hpp"
#include "Parallelogram3D.hpp"

namespace EARenderer {
    
    struct Ray3D {
        glm::vec3 origin;
        glm::vec3 direction;
        
        Ray3D(const glm::vec3& origin, const glm::vec3& end);
        
        /**
         Checks for an intersection with axis-alligned bounding box
         
         @param aab axis-alligned box
         @param distance distance from origin to intersection point
         @return return true if an intersection occurred
         */
        bool intersectsAAB(const AxisAlignedBox3D& aab, float& distance) const;
        bool intersectsParallelogram(const Parallelogram3D& parallelogram, float& distance) const;
        
        Ray3D transformedBy(const Transformation& t) const;
        Ray3D transformedBy(const glm::mat4& m) const;
    };
    
}

#endif /* Ray_hpp */
