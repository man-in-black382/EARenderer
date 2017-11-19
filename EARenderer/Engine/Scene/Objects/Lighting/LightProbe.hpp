//
//  LightProbe.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef LightProbe_hpp
#define LightProbe_hpp

#include "GLHDRTextureCubemap.hpp"

#include <glm/vec3.hpp>
#include <array>

namespace EARenderer {
    
    class LightProbe {
    private:
        
        
    public:
        using SphericalHarmonics = std::array<glm::vec3, 9>;
        
        glm::vec3 position;
        SphericalHarmonics SHCoefficients;
        
        void updateSphericalHarmonicsCoefficients(const GLHDRTextureCubemap& cubemap);
    };
    
}

#endif /* LightProbe_hpp */
