//
//  LightProbe.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef LightProbe_hpp
#define LightProbe_hpp

#include "SphericalHarmonics.hpp"
#include "GLHDRTextureCubemap.hpp"

#include <glm/vec3.hpp>

namespace EARenderer {
    
    class LightProbe {
    private:
        SphericalHarmonics mSphericalHarmonics;
        
        float solidAngle(int32_t x, int32_t y, const Size2D& cubeSize) const;
        float areaIntegral(float x, float y) const;
        
    public:
        glm::vec3 position;
        
        LightProbe(const glm::vec3& position);
        
        const SphericalHarmonics& sphericalHarmonics() const;
        void updateSHCoefficients(const GLHDRTextureCubemap& cubemap);
    };
    
}

#endif /* LightProbe_hpp */
