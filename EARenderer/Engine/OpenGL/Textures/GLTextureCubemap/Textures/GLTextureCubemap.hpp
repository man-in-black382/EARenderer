//
//  GLTextureCubemap.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLTextureCubemap_hpp
#define GLTextureCubemap_hpp

#include "GLTexture.hpp"
#include "GLCubemapFace.hpp"
#include "GLCubemapSampler.hpp"

#include <array>

namespace EARenderer {
    
    class GLTextureCubemap: public GLTexture {
    protected:
        void initialize(const Size2D& size, Filter filter, WrapMode wrapMode,
                        GLint internalFormat, GLenum format, GLenum type,
                        const std::array<void *, 6>& pixelData);

    public:
        GLTextureCubemap();
        virtual ~GLTextureCubemap() = 0;

        using SamplerClosure = const std::function<void(const GLCubemapSampler& sampler)>&;

        virtual void sampleTexels(int32_t mipLevel, SamplerClosure samplerClosure) const;
    };
    
}

#endif /* GLTextureCubemap_hpp */
