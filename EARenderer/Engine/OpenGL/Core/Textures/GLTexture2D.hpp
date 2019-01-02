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

    template<class TextureFormat, TextureFormat Format>
    class GLTexture2D : public GLTexture {
    protected:
        void initialize(const Size2D &size, Sampling::Filter filter, Sampling::WrapMode wrapMode, const void *pixelData) {
            if (size.width <= 0.0 || size.height <= 0.0) {
                throw std::invalid_argument("Texture size must not be zero");
            }

            mSize = size;
            constexpr GLTextureFormat f = glFormat(Format);

            glTexImage2D(GL_TEXTURE_2D, 0, f.internalFormat, size.width, size.height, 0, f.inputPixelFormat, f.inputPixelType, pixelData);

            setFilter(filter);
            setWrapMode(wrapMode);
        }

    public:
        GLTexture2D() : GLTexture(GL_TEXTURE_2D) {};

        virtual ~GLTexture2D() = 0;

        auto sampleTexels(uint8_t mipLevel = 0) const {
            return GLTexture2DSampler<TextureFormat, Format>(*this, mipLevel);
        }
    };

    template<class TextureFormat, TextureFormat Format>
    GLTexture2D<TextureFormat, Format>::~GLTexture2D() = default;


    template<GLTexture::Normalized Format>
    class GLNormalizedTexture2D : public GLTexture2D<GLTexture::Normalized, Format> {
    public:
        GLNormalizedTexture2D(const Size2D &size,
                const void *data = nullptr,
                Sampling::Filter filter = Sampling::Filter::Bilinear,
                Sampling::WrapMode wrapMode = Sampling::WrapMode::ClampToEdge) {
            this->initialize(size, filter, wrapMode, data);
        }

        ~GLNormalizedTexture2D() = default;
    };


    template<GLTexture::Integer Format>
    class GLIntegerTexture2D : public GLTexture2D<GLTexture::Integer, Format> {
    public:
        GLIntegerTexture2D(const Size2D &size, const void *data = nullptr) {
            this->initialize(size, Sampling::Filter::None, Sampling::WrapMode::ClampToEdge, data);
        }

        ~GLIntegerTexture2D() = default;
    };


    template<GLTexture::Float Format>
    class GLFloatTexture2D : public GLTexture2D<GLTexture::Float, Format> {
    public:
        GLFloatTexture2D(const Size2D &size,
                const void *data = nullptr,
                Sampling::Filter filter = Sampling::Filter::Bilinear,
                Sampling::WrapMode wrapMode = Sampling::WrapMode::ClampToEdge) {
            this->initialize(size, filter, wrapMode, data);
        }

        ~GLFloatTexture2D() = default;
    };


    class GLDepthTexture2D : public GLTexture2D<GLTexture::Depth, GLTexture::Depth::Default> {
    public:
        GLDepthTexture2D(const Size2D &size, Sampling::ComparisonMode comparisonMode = Sampling::ComparisonMode::None) {
            initialize(size, Sampling::Filter::Bilinear, Sampling::WrapMode::ClampToEdge, nullptr);
            setComparisonMode(comparisonMode);
        }

        ~GLDepthTexture2D() = default;
    };

}

#endif /* GLTexture2D_hpp */
