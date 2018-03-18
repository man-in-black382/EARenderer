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

#include <memory>

namespace EARenderer {

    class GLHDRTextureCubemap;

    class GLHDRCubemapSampler: public GLCubemapSampler {
    private:
        friend GLHDRTextureCubemap;

        std::unique_ptr<GLhalf> mPositiveXPixelBuffer;
        std::unique_ptr<GLhalf> mNegativeXPixelBuffer;
        std::unique_ptr<GLhalf> mPositiveYPixelBuffer;
        std::unique_ptr<GLhalf> mNegativeYPixelBuffer;
        std::unique_ptr<GLhalf> mPositiveZPixelBuffer;
        std::unique_ptr<GLhalf> mNegativeZPixelBuffer;
        
        GLHDRCubemapSampler(const GLHDRTextureCubemap& texture, uint8_t mipLevel);
        
    public:        
        Color sample(const glm::vec3& direction) const override;
        Color sample(GLCubemapFace face, int32_t x, int32_t y) const override;
    };
    
}

#endif /* GLHDRCubemapSampler_hpp */
