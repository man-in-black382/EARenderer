//
//  PointLight.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 25.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef PointLight_hpp
#define PointLight_hpp

#include "Light.hpp"
#include "Color.hpp"
#include "AxisAlignedBox3D.hpp"

#include <array>

#include <glm/vec3.hpp>

namespace EARenderer {
    
    class PointLight: public Light {
    private:
        float mRadius = 1.0;
        float mNearPlane = 1.0;
        
    public:
        PointLight(float radius, float nearClipPlane);
        
        float nearClipPlane() const;
        float farClipPlane() const;
        float radius() const;
        
        void setRadius(float radius);
        void setNearClipPlane(float nearPlane);
        
        std::array<glm::mat4, 6> viewProjectionMatrices() const;
    };
    
}

#endif /* PointLight_hpp */
