//
//  GLTexture2D.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 19.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLTexture2D_hpp
#define GLTexture2D_hpp

#include "GLTexture.hpp"
#include "GLTexture2DSampler.hpp"

namespace EARenderer {

    class GLTexture2D: public GLTexture {
    protected:
        void initialize(const Size2D& size, Filter filter, WrapMode wrapMode,
                        GLint internalFormat, GLenum format, GLenum type, const void *pixelData = nullptr);

    public:
        GLTexture2D();
        virtual ~GLTexture2D() = 0;

        using SamplerClosure = const std::function<void(const GLTexture2DSampler& sampler)>&;

        virtual void sampleTexels(int32_t mipLevel, SamplerClosure samplerClosure) const;
    };
    
}

#endif /* GLTexture2D_hpp */
