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

#include <array>

#include <glm/vec3.hpp>

namespace EARenderer {
    
    class PointLight: public Light {
    protected:
        float mClipDistance = 20.f;
        
    public:
        using Light::Light;
        PointLight(const glm::vec3& position, const Color& color, float clipDistance);
        
        std::array<glm::mat4, 6> viewProjectionMatrices() const;
        float clipDistance() const;
    };
    
}

#endif /* PointLight_hpp */
