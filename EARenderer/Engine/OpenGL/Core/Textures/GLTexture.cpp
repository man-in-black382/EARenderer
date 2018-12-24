//
//  GLTexture.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 06.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLTexture.hpp"
#include "GLTextureUnitManager.hpp"

#include <cmath>
#include <OpenGL/OpenGL.h>

namespace EARenderer {

#pragma mark - Lifecycle

    GLTexture::GLTexture(GLenum bindingPoint)
            : GLTexture(Size2D(1), bindingPoint) {
    }

    GLTexture::GLTexture(const Size2D &size, GLenum bindingPoint)
            : mSize(size), mBindingPoint(bindingPoint) {
        glGenTextures(1, &mName);
        GLTextureUnitManager::Shared().bindTextureToActiveUnit(*this);
    }

    GLTexture::~GLTexture() {
        glDeleteTextures(1, &mName);
    }

#pragma mark - Protected helpers

    void GLTexture::setFilter(Sampling::Filter filter) {
        GLint glMinFilter = 0;
        GLint glMagFilter = 0;

        switch (filter) {
            case Sampling::Filter::None:
                glMinFilter = glMagFilter = GL_NEAREST;
                break;
            case Sampling::Filter::Bilinear:
                glMinFilter = glMagFilter = GL_LINEAR;
                break;
            case Sampling::Filter::Trilinear:
                glMinFilter = GL_LINEAR_MIPMAP_LINEAR;
                glMagFilter = GL_LINEAR;
                break;
            case Sampling::Filter::Anisotropic:
                glMinFilter = GL_LINEAR_MIPMAP_LINEAR;
                glMagFilter = GL_LINEAR;
                float aniso = 0.0f;
                glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, std::min(aniso, 8.0f));
                break;
        }

        glTexParameteri(mBindingPoint, GL_TEXTURE_MIN_FILTER, glMinFilter);
        glTexParameteri(mBindingPoint, GL_TEXTURE_MAG_FILTER, glMagFilter);
    }

    void GLTexture::setWrapMode(Sampling::WrapMode wrapMode) {
        GLint wrap = 0;

        switch (wrapMode) {
            case Sampling::WrapMode::Repeat:
                wrap = GL_REPEAT;
                break;
            case Sampling::WrapMode::ClampToEdge:
                wrap = GL_CLAMP_TO_EDGE;
                break;
            case Sampling::WrapMode::ClampToBorder:
                wrap = GL_CLAMP_TO_BORDER;
                break;
        }

        glTexParameteri(mBindingPoint, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(mBindingPoint, GL_TEXTURE_WRAP_T, wrap);
        glTexParameteri(mBindingPoint, GL_TEXTURE_WRAP_R, wrap);
    }

    void GLTexture::setComparisonMode(Sampling::ComparisonMode comparisonMode) {
        switch (comparisonMode) {
            case Sampling::ComparisonMode::None:
                glTexParameteri(mBindingPoint, GL_TEXTURE_COMPARE_MODE, GL_NONE);
                break;
            case Sampling::ComparisonMode::ReferenceToTexture:
                glTexParameteri(mBindingPoint, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                glTexParameteri(mBindingPoint, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
                break;
        }
    }


#pragma mark - Static

    glm::vec2 GLTexture::WrapCoordinates(const glm::vec2 &uv) {
        float uFract = uv.x - (long) uv.x;
        float vFract = uv.y - (long) uv.y;

        uFract += 1;
        vFract += 1;

        return {uFract - (long) uFract, vFract - (long) vFract};
    }

    glm::vec3 GLTexture::WrapCoordinates(const glm::vec3 &uvr) {
        float uFract = uvr.x - (long) uvr.x;
        float vFract = uvr.y - (long) uvr.y;
        float rFract = uvr.z - (long) uvr.z;

        uFract += 1;
        vFract += 1;
        rFract += 1;

        return {uFract - (long) uFract, vFract - (long) vFract, rFract - (long) rFract};
    }

    Size2D GLTexture::EstimatedSize(size_t texelCount) {
        size_t dimensionLength = std::ceil(std::sqrt((float) texelCount));
        return Size2D(dimensionLength);
    }

    glm::vec2 GLTexture::UVMap(const glm::vec3 &vertex, const glm::vec3 &normal) {
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

    Size2D GLTexture::EstimatedMipSize(const Size2D &textureSize, uint8_t mipLevel) {
        return textureSize.transformedBy(glm::vec2(std::pow(0.5, mipLevel)));
    }

#pragma mark - Getters

    const Size2D &GLTexture::size() const {
        return mSize;
    }

    uint16_t GLTexture::mipMapCount() const {
        return mMipMapsCount;
    }

    GLenum GLTexture::bindingPoint() const {
        return mBindingPoint;
    }

#pragma mark - Mip Maps

    void GLTexture::generateMipMaps(size_t count) {
        GLTextureUnitManager::Shared().bindTextureToActiveUnit(*this);
        glTexParameteri(mBindingPoint, GL_TEXTURE_MAX_LEVEL, GLint(count));
        setFilter(Sampling::Filter::Trilinear);
        glGenerateMipmap(mBindingPoint);

        mMipMapsCount = floor(std::log2(std::max(mSize.width, mSize.height)));
        mMipMapsCount = std::min(mMipMapsCount, uint16_t(count));
    }

    Size2D GLTexture::mipMapSize(size_t mipLevel) const {
        GLTextureUnitManager::Shared().bindTextureToActiveUnit(*this);
        GLint w = 0;
        GLint h = 0;
        glGetTexLevelParameteriv(mBindingPoint, GLint(mipLevel), GL_TEXTURE_WIDTH, &w);
        glGetTexLevelParameteriv(mBindingPoint, GLint(mipLevel), GL_TEXTURE_HEIGHT, &h);
        return {float(w), float(h)};
    }

}
