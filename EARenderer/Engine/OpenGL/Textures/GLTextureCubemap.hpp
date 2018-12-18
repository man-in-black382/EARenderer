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

#include <array>
#include <OpenGL/gl3.h>
#include <stdexcept>

namespace EARenderer {

    template<class TextureFormat, TextureFormat Format>
    class GLTextureCubemap : public GLTexture {
    protected:
        void initialize(const Size2D &size, Sampling::Filter filter, Sampling::WrapMode wrapMode, const std::array<const void *, 6> &pixelData) {
            if ((size_t) size.width != (size_t) size.height) {
                throw std::invalid_argument("Cubemap width and height must be equal");
            }

            mSize = size;
            constexpr GLTextureFormat f = glFormat(Format);

            for (GLuint i = 0; i < 6; i++) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, f.internalFormat, size.width,
                        size.height, 0, f.inputPixelFormat, f.inputPixelType, pixelData[i]);
            }

            setFilter(filter);
            setWrapMode(wrapMode);
        }

    public:
        GLTextureCubemap() : GLTexture(GL_TEXTURE_CUBE_MAP) {
        };

        virtual ~GLTextureCubemap() = 0;
    };

    template<class TextureFormat, TextureFormat Format>
    GLTextureCubemap<TextureFormat, Format>::~GLTextureCubemap() = default;


    template<GLTexture::Normalized Format>
    class GLNormalizedTextureCubemap : public GLTextureCubemap<GLTexture::Normalized, Format> {
    public:
        GLNormalizedTextureCubemap(const Size2D &size,
                const std::array<const void *, 6> &pixelData = {},
                Sampling::Filter filter = Sampling::Filter::Bilinear,
                Sampling::WrapMode wrapMode = Sampling::WrapMode::ClampToEdge) {
            this->initialize(size, filter, wrapMode, pixelData);
        }

        ~GLNormalizedTextureCubemap() = default;
    };


    template<GLTexture::Integer Format>
    class GLIntegerTextureCubemap : public GLTextureCubemap<GLTexture::Integer, Format> {
    public:
        GLIntegerTextureCubemap(const Size2D &size, const std::array<const void *, 6> &pixelData = {}) {
            this->initialize(size, Sampling::Filter::None, Sampling::WrapMode::ClampToEdge, pixelData);
        }

        ~GLIntegerTextureCubemap() = default;
    };


    template<GLTexture::Float Format>
    class GLFloatTextureCubemap : public GLTextureCubemap<GLTexture::Float, Format> {
    public:
        GLFloatTextureCubemap(const Size2D &size,
                const std::array<const void *, 6> &pixelData = {},
                Sampling::Filter filter = Sampling::Filter::Bilinear,
                Sampling::WrapMode wrapMode = Sampling::WrapMode::ClampToEdge) {
            this->initialize(size, filter, wrapMode, pixelData);
        }

        ~GLFloatTextureCubemap() = default;
    };


    class GLDepthTextureCubemap : public GLTextureCubemap<GLTexture::Depth, GLTexture::Depth::Default> {
    public:
        GLDepthTextureCubemap(const Size2D &size, Sampling::ComparisonMode comparisonMode = Sampling::ComparisonMode::None) {
            initialize(size, Sampling::Filter::Bilinear, Sampling::WrapMode::ClampToEdge, {});
            setComparisonMode(comparisonMode);
        }

        ~GLDepthTextureCubemap() = default;
    };

}

#endif /* GLTextureCubemap_hpp */
