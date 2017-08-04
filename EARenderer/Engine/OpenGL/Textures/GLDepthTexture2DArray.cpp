//
//  GLTexture2DArray.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 02.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLDepthTexture2DArray.hpp"
#include "Macros.h"

#include <OpenGL/gl3ext.h>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLDepthTexture2DArray::GLDepthTexture2DArray(const Size2D& size, uint16_t capacity)
    :
    mSize(size),
    mCapacity(capacity)
    {
        ASSERT(size.width > 0, "Depth texture 2d array width should be greater than 0");
        ASSERT(size.height > 0, "Depth texture 2d array height should be greater than 0");
        
        GLuint name = -1;
        glGenTextures(1, &name);
        mName = name;
        
        bind();
        
        glTexStorage3D(GL_TEXTURE_2D_ARRAY,
                       1, // No mipmaps (1 means that there is only one base image level)
                       GL_DEPTH_COMPONENT16, // Internal format
                       size.width, // Width
                       size.height, // Height
                       capacity); // Number of layers (elements, textures) in the array
        
        applyParameters();
    }
    
    GLDepthTexture2DArray::~GLDepthTexture2DArray() {
        glDeleteTextures(1, &mName);
    }
    
    void GLDepthTexture2DArray::applyParameters() {
//        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//        // Prevent oversampling by supplying 1.0 depth values when texture is sampled beyond [0.0; 1.0] range
//        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
//        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
        
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    
#pragma mark - Getters
    
    const Size2D& GLDepthTexture2DArray::size() const {
        return mSize;
    }
    
    uint16_t GLDepthTexture2DArray::capacity() const {
        return mCapacity;
    }
   
#pragma mark - Binding
    
    void GLDepthTexture2DArray::bind() const {
        glBindTexture(GL_TEXTURE_2D_ARRAY, mName);
    }
    
}
