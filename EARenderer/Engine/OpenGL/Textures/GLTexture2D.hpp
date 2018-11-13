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

    template <class TextureFormat, TextureFormat Format>
    class GLTexture2D: public GLTexture {
    protected:
        void initialize(const Size2D& size, Filter filter, WrapMode wrapMode, const void *pixelData)
        {
            if (size.width <= 0.0 || size.height <= 0.0) {
                throw std::invalid_argument("Texture size must not be zero");
            }

            mSize = size;
            GLTextureFormat f = glFormat(Format);

            glTexImage2D(GL_TEXTURE_2D, 0, f.internalFormat, size.width, size.height, 0, f.inputPixelFormat, f.inputPixelType, pixelData);

            setFilter(filter);
            setWrapMode(wrapMode);
        }

    public:
        GLTexture2D() : GLTexture(GL_TEXTURE_2D) { };

        virtual ~GLTexture2D() = 0;

        auto sampleTexels(uint8_t mipLevel = 0) const {
            return GLTexture2DSampler<TextureFormat, Format>(*this, mipLevel);
        }
    };

    template <class TextureFormat, TextureFormat Format>
    GLTexture2D<TextureFormat, Format>::~GLTexture2D() = default;

    
    template<GLTexture::Normalized Format>
    class GLNormalizedTexture2D: public GLTexture2D<GLTexture::Normalized, Format> {
    public:
        GLNormalizedTexture2D(const Size2D& size,
                              const void *data = nullptr,
                              GLTexture::Filter filter = GLTexture::Filter::Bilinear,
                              GLTexture::WrapMode wrapMode = GLTexture::WrapMode::ClampToEdge)
        {
            this->initialize(size, filter, wrapMode, data);
        }

        ~GLNormalizedTexture2D() = default;
    };


    template<GLTexture::Integer Format>
    class GLIntegerTexture2D: public GLTexture2D<GLTexture::Integer, Format> {
    public:
        GLIntegerTexture2D(const Size2D& size, const void *data = nullptr) {
            this->initialize(size, GLTexture::Filter::None, GLTexture::WrapMode::ClampToEdge, data);
        }

        ~GLIntegerTexture2D() = default;
    };


    template<GLTexture::Float Format>
    class GLFloatTexture2D: public GLTexture2D<GLTexture::Float, Format> {
    public:
        GLFloatTexture2D(const Size2D& size,
                         const void *data = nullptr,
                         GLTexture::Filter filter = GLTexture::Filter::Bilinear,
                         GLTexture::WrapMode wrapMode = GLTexture::WrapMode::ClampToEdge)
        {
            this->initialize(size, filter, wrapMode, data);
        }

        ~GLFloatTexture2D() = default;
    };


    class GLDepthTexture2D: public GLTexture2D<GLTexture::Depth, GLTexture::Depth::Default> {
    public:
        GLDepthTexture2D(const Size2D& size) {
            initialize(size, Filter::Bilinear, WrapMode::ClampToBorder, nullptr);
            // Prevent oversampling by supplying 1.0 depth values when texture is sampled beyond [0.0; 1.0] range
            GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        }

        ~GLDepthTexture2D() = default;
    };

}

#endif /* GLTexture2D_hpp */
