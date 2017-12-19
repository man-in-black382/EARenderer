//
//  GLHDRCubemapSampler.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLHDRCubemapSampler_hpp
#define GLHDRCubemapSampler_hpp

#include "GLCubemapSampler.hpp"
#include "GLHDRTextureCubemap.hpp"

namespace EARenderer {
    
    class GLHDRCubemapSampler: public GLCubemapSampler {
    private:
        friend GLHDRTextureCubemap;
        
        const GLHDRTextureCubemap *mCubemap;
        const GLhalf *mPositiveXPixelBuffer = nullptr;
        const GLhalf *mNegativeXPixelBuffer = nullptr;
        const GLhalf *mPositiveYPixelBuffer = nullptr;
        const GLhalf *mNegativeYPixelBuffer = nullptr;
        const GLhalf *mPositiveZPixelBuffer = nullptr;
        const GLhalf *mNegativeZPixelBuffer = nullptr;
        
        GLHDRCubemapSampler(const GLHDRTextureCubemap* cubemap);
        
    public:
        ~GLHDRCubemapSampler();
        
        Color sample(const glm::vec3& direction) const override;
        Color sample(GLCubemapFace face, int32_t x, int32_t y) const override;
    };
    
}

#endif /* GLHDRCubemapSampler_hpp */
