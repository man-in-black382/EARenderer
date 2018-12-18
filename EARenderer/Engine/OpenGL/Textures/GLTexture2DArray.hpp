//
//  GLTexture2DArray.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 02.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLTexture2DArray_hpp
#define GLTexture2DArray_hpp

#include "GLTexture.hpp"

#include <OpenGL/gl3ext.h>
#include <vector>

namespace EARenderer {

    template<class TextureFormat, TextureFormat Format>
    class GLTexture2DArray : public GLTexture {
    protected:

        size_t mCount;

        void initialize(const Size2D &size, size_t count, const std::vector<const void *> &pixelData, Sampling::Filter filter, Sampling::WrapMode wrapMode) {

            if (size.width <= 0.0 || size.height <= 0.0) {
                throw std::invalid_argument("Texture size must not be zero");
            }

            if (count == 0) {
                throw std::invalid_argument("Allocating a zero-sized texture array");
            }

            mCount = count;
            mSize = size;

            constexpr GLTextureFormat f = glFormat(Format);

            glTexStorage3D(GL_TEXTURE_2D_ARRAY,
                    1, // No mipmaps (1 means that there is only one base image level)
                    f.internalFormat, // Internal format
                    size.width, // Width
                    size.height, // Height
                    (GLint) count); // Number of layers (elements, textures) in the array

            for (size_t i = 0; i < pixelData.size(); i++) {
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, (GLint) i,
                        size.width, size.height, 1,
                        f.inputPixelFormat, f.inputPixelType, pixelData[i]);
            }

            setFilter(filter);
            setWrapMode(wrapMode);
        }

    public:
        GLTexture2DArray() : GLTexture(GL_TEXTURE_2D_ARRAY) {
        };

        virtual ~GLTexture2DArray() = 0;
    };

    template<class TextureFormat, TextureFormat Format>
    GLTexture2DArray<TextureFormat, Format>::~GLTexture2DArray() = default;


    template<GLTexture::Normalized Format>
    class GLNormalizedTexture2DArray : public GLTexture2DArray<GLTexture::Normalized, Format> {
    public:
        GLNormalizedTexture2DArray(const Size2D &size,
                size_t count,
                const std::vector<const void *> &pixelData = {},
                Sampling::Filter filter = Sampling::Filter::Bilinear,
                Sampling::WrapMode wrapMode = Sampling::WrapMode::ClampToEdge) {
            this->initialize(size, count, pixelData, filter, wrapMode);
        }

        ~GLNormalizedTexture2DArray() = default;
    };


    template<GLTexture::Integer Format>
    class GLIntegerTexture2DArray : public GLTexture2DArray<GLTexture::Integer, Format> {
    public:
        GLIntegerTexture2DArray(const Size2D &size, size_t count, const std::vector<const void *> &pixelData = {}) {
            this->initialize(size, count, pixelData, Sampling::Filter::None, Sampling::WrapMode::ClampToEdge);
        }

        ~GLIntegerTexture2DArray() = default;
    };


    template<GLTexture::Float Format>
    class GLFloatTexture2DArray : public GLTexture2DArray<GLTexture::Float, Format> {
    public:
        GLFloatTexture2DArray(const Size2D &size,
                size_t count,
                const std::vector<const void *> &pixelData = {},
                Sampling::Filter filter = Sampling::Filter::Bilinear,
                Sampling::WrapMode wrapMode = Sampling::WrapMode::ClampToEdge) {
            this->initialize(size, count, pixelData, filter, wrapMode);
        }

        ~GLFloatTexture2DArray() = default;
    };


    class GLDepthTexture2DArray : public GLTexture2DArray<GLTexture::Depth, GLTexture::Depth::Default> {
    public:
        GLDepthTexture2DArray(const Size2D &size, size_t count, Sampling::ComparisonMode comparisonMode = Sampling::ComparisonMode::None) {
            initialize(size, count, {}, Sampling::Filter::Bilinear, Sampling::WrapMode::ClampToEdge);
            setComparisonMode(comparisonMode);
        }

        ~GLDepthTexture2DArray() = default;
    };

}

#endif /* GLTexture2DArray_hpp */
