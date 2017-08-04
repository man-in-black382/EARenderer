//
//  DirectionalLight.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef DirectionalLight_hpp
#define DirectionalLight_hpp

#include "Light.hpp"

namespace EARenderer {
    
    class DirectionalLight: public Light {
    protected:
        glm::vec3 mDirection;
        float mFrustumSize = 20.f;
        
    public:
        DirectionalLight(const Color& color, const glm::vec3& direction);
        DirectionalLight(const Color& color, const glm::vec3& direction, float frustumSize);
        
        glm::mat4 viewMatrix() const;
        glm::mat4 projectionMatrix() const;
        glm::mat4 viewProjectionMatrix() const;
        
        const glm::vec3& direction() const;
        void setDirection(const glm::vec3& direction);
    };
    
}

#endif /* DirectionalLight_hpp */
