//
//  FrustumCascadeBuilder.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 01.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef FrustumCascadeBuilder_hpp
#define FrustumCascadeBuilder_hpp

#include "Camera.hpp"
#include "DirectionalLight.hpp"
#include "FrustumCascades.hpp"

#include <vector>

#include <glm/mat4x4.hpp>

namespace EARenderer {
    
    class FrustumCascadeBuilder {
    private:
        static float split(uint8_t cascadeIndex, uint8_t cascadesCount, float nearPlane, float farPlane, float lambda);
        
    public:
        static FrustumCascades subfrustumsForCameraInLightSpace(const Camera& camera, const DirectionalLight& light, int8_t numberOfCascades);
    };
    
}

#endif /* FrustumCascadeBuilder_hpp */
