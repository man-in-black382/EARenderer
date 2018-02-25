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
    
    GLTexture::GLTexture(GLenum bindingPoint, GLint minFilter, GLint magFilter, GLint wrapModeS, GLint wrapModeT)
    :
    GLTexture(Size2D::zero(), bindingPoint, minFilter, magFilter, wrapModeS, wrapModeT)
    { }
    
    GLTexture::GLTexture(const Size2D& size, GLenum bindingPoint, GLint minFilter, GLint magFilter, GLint wrapModeS, GLint wrapModeT)
    :
    mSize(size),
    mBindingPoint(bindingPoint)
    {
        glGenTextures(1, &mName);
        bind();
        glTexParameteri(mBindingPoint, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(mBindingPoint, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(mBindingPoint, GL_TEXTURE_WRAP_S, wrapModeS);
        glTexParameteri(mBindingPoint, GL_TEXTURE_WRAP_T, wrapModeT);
    }
    
    GLTexture::~GLTexture() {
        glDeleteTextures(1, &mName);
    }

#pragma mark - Static

    glm::vec2 GLTexture::wrapCoordinates(const glm::vec2& uv) {
        float uFract = uv.x - (long)uv.x;
        float vFract = uv.y - (long)uv.y;

        uFract += 1;
        vFract += 1;

        return { uFract - (long)uFract, vFract - (long)vFract };
    }

    glm::vec3 GLTexture::wrapCoordinates(const glm::vec3& uvr) {
        float uFract = uvr.x - (long)uvr.x;
        float vFract = uvr.y - (long)uvr.y;
        float rFract = uvr.z - (long)uvr.z;

        uFract += 1;
        vFract += 1;
        rFract += 1;

        return { uFract - (long)uFract, vFract - (long)vFract, rFract - (long)rFract };
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
        glTexParameteri(mBindingPoint, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(mBindingPoint);
        mMipMapsCount = floor(std::log2(std::max(mSize.width, mSize.height)));
    }
    
}
