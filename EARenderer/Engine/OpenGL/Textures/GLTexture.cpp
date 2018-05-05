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
    
    void GLTexture::generateMipmaps() {
        bind();
//        glTexParameteri(mBindingPoint, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(mBindingPoint);
        mMipMapsCount = floor(std::log2(std::max(mSize.width, mSize.height)));

        GLint maxLevel = 1;
        glGetTexParameteriv(mBindingPoint, GL_TEXTURE_MAX_LEVEL, &maxLevel);
        mMipMapsCount = std::min(mMipMapsCount, (uint16_t)maxLevel);
    }
    
}
