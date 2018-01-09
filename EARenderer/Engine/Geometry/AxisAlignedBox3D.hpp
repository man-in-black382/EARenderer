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

#include <array>

namespace EARenderer {
    
    struct AxisAlignedBox3D {
        glm::vec3 min = glm::zero<glm::vec3>();
        glm::vec3 max = glm::zero<glm::vec3>();
        
        static const AxisAlignedBox3D& zero();
        static const AxisAlignedBox3D& unit();
        static const AxisAlignedBox3D& maximum();
        
        AxisAlignedBox3D()= default;
        AxisAlignedBox3D(const glm::vec3& min, const glm::vec3& max);
        
        float diagonal() const;
        
        /**
         Represents box as an orthographic projection matrix
         as if it was diretional light's frustum, for example

         @return orthographic projection matrix
         */
        glm::mat4 asFrustum() const;
        std::array<glm::vec4, 8> cornerPoints() const;
        
        AxisAlignedBox3D transformedBy(const Transformation& t) const;
        AxisAlignedBox3D transformedBy(const glm::mat4& m) const;
        bool containsPoint(const glm::vec3& point);
    };
        
}

#endif /* Box_hpp */
