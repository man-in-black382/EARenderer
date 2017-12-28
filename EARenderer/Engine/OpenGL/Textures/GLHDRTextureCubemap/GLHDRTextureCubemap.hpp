//
//  GLHDRTextureCubemap.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLHDRTextureCubemap_hpp
#define GLHDRTextureCubemap_hpp

#include "GLTexture.hpp"
#include "GLCubemapFace.hpp"
#include "GLCubemapSampler.hpp"

namespace EARenderer {
    
    class GLHDRTextureCubemap: public GLTexture {
    public:
        using SamplerClosure = const std::function<void(const GLCubemapSampler& sampler)>&;
        
        GLHDRTextureCubemap(const Size2D& size);
        
        const GLhalf* pixelBuffer(GLCubemapFace face) const;
        void sampleTexels(SamplerClosure samplerClosure) const;
    };
    
}

#endif /* GLHDRTextureCubemap_hpp */
