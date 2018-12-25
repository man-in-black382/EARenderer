//
//  GLTextureCubemapArray.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 8/29/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLTextureCubemapArray_hpp
#define GLTextureCubemapArray_hpp

#include "GLTexture.hpp"

#include <OpenGL/gl3ext.h>

namespace EARenderer {

    template<class TextureFormat, TextureFormat Format>
    class GLTextureCubemapArray : public GLTexture {
    protected:

        size_t mCount;

        void initialize(const Size2D &size, size_t count, Sampling::Filter filter, Sampling::WrapMode wrapMode) {

            if (size.width <= 0.0 || size.height <= 0.0) {
                throw std::invalid_argument("Texture size must not be zero");
            }

            if (count == 0) {
                throw std::invalid_argument("Allocating a zero-sized cubemap array");
            }

            mCount = count;
            mSize = size;

            constexpr GLTextureFormat f = glFormat(Format);

            glTexStorage3D(GL_TEXTURE_3D,
                    1, // No mipmaps (1 means that there is only one base image level)
                    f.internalFormat, // Internal format
                    size.width, // Width
                    size.height, // Height
                    (GLint) count * 6); // Number of layers * number of faces per layer

            setFilter(filter);
            setWrapMode(wrapMode);
        }

    public:
        GLTextureCubemapArray() : GLTexture(GL_TEXTURE_CUBE_MAP_ARRAY) {
        };

        virtual ~GLTextureCubemapArray() = 0;
    };

    template<class TextureFormat, TextureFormat Format>
    GLTextureCubemapArray<TextureFormat, Format>::~GLTextureCubemapArray() = default;


    template<GLTexture::Normalized Format>
    class GLNormalizedTextureCubemapArray : public GLTextureCubemapArray<GLTexture::Normalized, Format> {
    public:
        GLNormalizedTextureCubemapArray(const Size2D &size,
                size_t count,
                Sampling::Filter filter = Sampling::Filter::Bilinear,
                Sampling::WrapMode wrapMode = Sampling::WrapMode::ClampToEdge) {
            this->initialize(size, count, filter, wrapMode);
        }

        ~GLNormalizedTextureCubemapArray() = default;
    };


    template<GLTexture::Integer Format>
    class GLIntegerTextureCubemapArray : public GLTextureCubemapArray<GLTexture::Integer, Format> {
    public:
        GLIntegerTextureCubemapArray(const Size2D &size, size_t count) {
            this->initialize(size, count, Sampling::Filter::None, Sampling::WrapMode::ClampToEdge);
        }

        ~GLIntegerTextureCubemapArray() = default;
    };


    template<GLTexture::Float Format>
    class GLFloatTextureCubemapArray : public GLTextureCubemapArray<GLTexture::Float, Format> {
    public:
        GLFloatTextureCubemapArray(const Size2D &size,
                size_t count,
                Sampling::Filter filter = Sampling::Filter::Bilinear,
                Sampling::WrapMode wrapMode = Sampling::WrapMode::ClampToEdge) {
            this->initialize(size, count, filter, wrapMode);
        }

        ~GLFloatTextureCubemapArray() = default;
    };


    class GLDepthTextureCubemapArray : public GLTextureCubemapArray<GLTexture::Depth, GLTexture::Depth::Default> {
    public:
        GLDepthTextureCubemapArray(const Size2D &size, size_t count, Sampling::ComparisonMode comparisonMode = Sampling::ComparisonMode::None) {
            initialize(size, count, Sampling::Filter::Bilinear, Sampling::WrapMode::ClampToEdge);
            setComparisonMode(comparisonMode);
        }

        ~GLDepthTextureCubemapArray() = default;
    };

}

#endif /* GLTextureCubemapArray_hpp */
