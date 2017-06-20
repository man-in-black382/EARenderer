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

#include "Box.hpp"

namespace EARenderer {
    
    class Ray {
    private:
        glm::vec3 mOrigin;
        glm::vec3 mDirection;
        
    public:
        Ray(const glm::vec3& origin, const glm::vec3& end);
        
        const glm::vec3& origin() const;
        const glm::vec3& direction() const;
        
        /**
         Cheks for an intersection with an axis-alligned bounding box
         
         @param aabb axis-alligned bounding box
         @param distance distance from origin to intersection point
         @return return true if an intersection occurred
         */
        bool intersectsAABB(const Box& aabb, float *distance);
        
        Ray transformedBy(const Transformation& t) const;
        Ray transformedBy(const glm::mat4& m) const;
    };
    
}

#endif /* Ray_hpp */
