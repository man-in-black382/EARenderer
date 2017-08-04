//
//  Box.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 16.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Box_hpp
#define Box_hpp

#include "Transformation.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace EARenderer {
    
    struct AxisAlignedBox3D {
        glm::vec3 min;
        glm::vec3 max;
        
        static const AxisAlignedBox3D& zero();
        static const AxisAlignedBox3D& unit();
        
        AxisAlignedBox3D();
        AxisAlignedBox3D(const glm::vec3& min, const glm::vec3& max);
        
        const float diagonal() const;
        
        /**
         Represents box as an orthographic projection matrix
         as if it was diretional light's frustum, for example

         @return orthographic projection matrix
         */
        glm::mat4 asFrustum() const;
        
        AxisAlignedBox3D transformedBy(const Transformation& t) const;
        AxisAlignedBox3D transformedBy(const glm::mat4& m) const;
    };
        
}

#endif /* Box_hpp */
