//
//  GLTexture.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 06.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLTexture.hpp"
#include "GLTexture2D.hpp"
#include "GLTexture2DArray.hpp"

#include <cmath>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLTexture::GLTexture(GLenum bindingPoint)
    :
    GLTexture(Size2D::zero(), bindingPoint)
    { }

    GLTexture::GLTexture(const Size2D& size, GLenum bindingPoint)
    :
    mSize(size),
    mBindingPoint(bindingPoint)
    {
        glGenTextures(1, &mName);
        bind();
    }

    GLTexture::~GLTexture() {
        glDeleteTextures(1, &mName);
    }

#pragma mark - Protected helpers

    void GLTexture::setFilter(Filter filter) {
        GLint glMinFilter = 0;
        GLint glMagFilter = 0;

        switch (filter) {
            case Filter::None: glMinFilter = glMagFilter = GL_NEAREST; break;
            case Filter::Bilinear: glMinFilter = glMagFilter = GL_LINEAR; break;
            case Filter::Trilinear: glMinFilter = GL_LINEAR_MIPMAP_LINEAR; glMagFilter = GL_LINEAR; break;
        }

        glTexParameteri(mBindingPoint, GL_TEXTURE_MIN_FILTER, glMinFilter);
        glTexParameteri(mBindingPoint, GL_TEXTURE_MAG_FILTER, glMagFilter);
    }

    void GLTexture::setWrapMode(WrapMode wrapMode) {
        GLint wrap = 0;

        switch (wrapMode) {
            case WrapMode::Repeat: wrap = GL_REPEAT; break;
            case WrapMode::ClampToEdge: wrap = GL_CLAMP_TO_EDGE; break;
            case WrapMode::ClampToBorder: wrap = GL_CLAMP_TO_BORDER; break;
        }

        glTexParameteri(mBindingPoint, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(mBindingPoint, GL_TEXTURE_WRAP_T, wrap);
        glTexParameteri(mBindingPoint, GL_TEXTURE_WRAP_R, wrap);
    }

    GLTextureFormat GLTexture::glFormat(Normalized format) {
        switch (format) {
            case Normalized::RCompressed:     return { GL_COMPRESSED_RED,  GL_RGBA, GL_UNSIGNED_BYTE };
            case Normalized::RGCompressed:    return { GL_COMPRESSED_RG,   GL_RGBA, GL_UNSIGNED_BYTE };
            case Normalized::RGBCompressed:   return { GL_COMPRESSED_RGB,  GL_RGBA, GL_UNSIGNED_BYTE };
            case Normalized::RGBACompressed:  return { GL_COMPRESSED_RGBA, GL_RGBA, GL_UNSIGNED_BYTE };
        }
    }

    GLTextureFormat GLTexture::glFormat(Integer format){
        switch (format) {
            case Integer::R32UI:      return { GL_R32UI,    GL_RED_INTEGER,  GL_UNSIGNED_INT };
            case Integer::RG32UI:     return { GL_RG32UI,   GL_RG_INTEGER,   GL_UNSIGNED_INT };
            case Integer::RGB32UI:    return { GL_RGB32UI,  GL_RGB_INTEGER,  GL_UNSIGNED_INT };
            case Integer::RGBA32UI:   return { GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT };
        }
    }

    GLTextureFormat GLTexture::glFormat(Float format) {
        switch (format) {
            case Float::R16F:     return { GL_R16F,     GL_RED,  GL_FLOAT };
            case Float::RG16F:    return { GL_RG16F,    GL_RG,   GL_FLOAT };
            case Float::RGB16F:   return { GL_RGB16F,   GL_RGB,  GL_FLOAT };
            case Float::RGBA16F:  return { GL_RGBA16F,  GL_RGBA, GL_FLOAT };
            case Float::R32F:     return { GL_R32F,     GL_RED,  GL_FLOAT };
            case Float::RG32F:    return { GL_RG32F,    GL_RG,   GL_FLOAT };
            case Float::RGB32F:   return { GL_RGB32F,   GL_RGB,  GL_FLOAT };
            case Float::RGBA32F:  return { GL_RGBA32F,  GL_RGBA, GL_FLOAT };
        }
    }

    GLTextureFormat GLTexture::glFormat(Depth format) {
        switch (format) {
            case Depth::Default: return { GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT };
        }
    }

#pragma mark - Static

    glm::vec2 GLTexture::WrapCoordinates(const glm::vec2& uv) {
        float uFract = uv.x - (long)uv.x;
        float vFract = uv.y - (long)uv.y;

        uFract += 1;
        vFract += 1;

        return { uFract - (long)uFract, vFract - (long)vFract };
    }

    glm::vec3 GLTexture::WrapCoordinates(const glm::vec3& uvr) {
        float uFract = uvr.x - (long)uvr.x;
        float vFract = uvr.y - (long)uvr.y;
        float rFract = uvr.z - (long)uvr.z;

        uFract += 1;
        vFract += 1;
        rFract += 1;

        return { uFract - (long)uFract, vFract - (long)vFract, rFract - (long)rFract };
    }

    Size2D GLTexture::EstimatedSize(size_t texelCount) {
        size_t dimensionLength = std::ceil(std::sqrt((float)texelCount));
        return Size2D(dimensionLength);
    }

    glm::vec2 GLTexture::UVMap(const glm::vec3& vertex, const glm::vec3& normal) {
        glm::vec2 uv;

        if (fabs(normal.x) > fabs(normal.y) && fabs(normal.x) > fabs(normal.z)) {
            uv.x = vertex.y;
            uv.y = vertex.z;
        } else if (fabs(normal.y) > fabs(normal.x) && fabs(normal.y) > fabs(normal.z)) {
            uv.x = vertex.x;
            uv.y = vertex.z;
        } else {
            uv.x = vertex.x;
            uv.y = vertex.y;
        }

        return uv;
    }

    Size2D GLTexture::EstimatedMipSize(const Size2D& textureSize, uint8_t mipLevel) {
        return textureSize.transformedBy(glm::vec2(std::pow(0.5, mipLevel)));
    }
    
#pragma mark - Getters
    
    const Size2D& GLTexture::size() const {
        return mSize;
    }
    
    uint16_t GLTexture::mipMapsCount() const {
        return mMipMapsCount;
    }
    
#pragma mark - Binding
    
    void GLTexture::bind() const {
        glBindTexture(mBindingPoint, mName);
    }
    
    void GLTexture::generateMipMaps(size_t count) {
        bind();
        glTexParameteri(mBindingPoint, GL_TEXTURE_MAX_LEVEL, GLint(count));
        setFilter(Filter::Trilinear);
        glGenerateMipmap(mBindingPoint);

        mMipMapsCount = floor(std::log2(std::max(mSize.width, mSize.height)));
        mMipMapsCount = std::min(mMipMapsCount, uint16_t(count));
    }

    Size2D GLTexture::mipMapSize(size_t mipLevel) const {
        bind();
        GLint w = 0;
        GLint h = 0;
        glGetTexLevelParameteriv(mBindingPoint, GLint(mipLevel), GL_TEXTURE_WIDTH, &w);
        glGetTexLevelParameteriv(mBindingPoint, GLint(mipLevel), GL_TEXTURE_HEIGHT, &h);
        return { float(w), float(h) };
    }
    
}
