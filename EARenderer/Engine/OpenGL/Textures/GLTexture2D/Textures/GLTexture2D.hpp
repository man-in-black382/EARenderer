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
    };

    class GLNormalizedTexture2D: public GLTexture2D {
    public:
        GLNormalizedTexture2D(const Size2D& size, NormalizedFormat format, void *data = nullptr,
                              Filter filter = Filter::Bilinear, WrapMode wrapMode = WrapMode::ClampToEdge);

        ~GLNormalizedTexture2D() = default;
    };

    class GLIntegerTexture2D: public GLTexture2D {
    public:
        GLIntegerTexture2D(const Size2D& size, IntegerFormat format, void *data = nullptr);
        ~GLIntegerTexture2D() = default;
    };

    class GLFloatTexture2D: public GLTexture2D {
    public:
        GLFloatTexture2D(const Size2D& size, FloatFormat format, void *data = nullptr,
                         Filter filter = Filter::Bilinear, WrapMode wrapMode = WrapMode::ClampToEdge);

        ~GLFloatTexture2D() = default;
    };

    class GLDepthTexture2D: public GLTexture2D {
    public:
        GLDepthTexture2D(const Size2D& size);
        ~GLDepthTexture2D() = default;
    };
    
}

#endif /* GLTexture2D_hpp */
