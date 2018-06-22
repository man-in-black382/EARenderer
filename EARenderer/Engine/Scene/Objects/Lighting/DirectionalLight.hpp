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
#include "Camera.hpp"
#include "FrustumCascades.hpp"

#include <vector>

namespace EARenderer {
    
    class DirectionalLight: public Light {
    protected:
        glm::vec3 mDirection;
        float mFrustumSize = 10.f;
        
    public:
        DirectionalLight(const Color& color, const glm::vec3& direction);
        DirectionalLight(const Color& color, const glm::vec3& direction, float frustumSize);
        
        glm::mat4 viewMatrix() const;
        FrustumCascades cascadesForCamera(const Camera& camera, uint8_t numberOfCascades, const glm::vec3& scale = glm::vec3(1.0)) const;
        FrustumCascades cascadesForWorldBoundingBox(const AxisAlignedBox3D& box) const;
        
        const glm::vec3& direction() const;
        void setDirection(const glm::vec3& direction);
    };
    
}

#endif /* DirectionalLight_hpp */
