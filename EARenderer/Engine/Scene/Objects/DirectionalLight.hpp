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
        
    public:
        DirectionalLight(const glm::vec3& position, const glm::vec3& color, const glm::vec3& direction);
        
        glm::mat4 viewProjectionMatrix() const;
        const glm::vec3& direction() const;
        void setDirection(const glm::vec3& direction);
    };
    
}

#endif /* DirectionalLight_hpp */
