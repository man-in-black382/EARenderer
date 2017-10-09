//
//  GLLayeredTexture.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLLayeredTexture.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLLayeredTexture::GLLayeredTexture(GLsizei layers, GLenum bindingPoint, GLint minFilter, GLint magFilter, GLint wrapModeS, GLint wrapModeT, GLint wrapModeR)
    :
    GLLayeredTexture(Size2D::zero(), layers, bindingPoint, minFilter, magFilter, wrapModeS, wrapModeT, wrapModeR)
    { }
    
    GLLayeredTexture::GLLayeredTexture(const Size2D& size, GLsizei layers, GLenum bindingPoint, GLint minFilter, GLint magFilter, GLint wrapModeS, GLint wrapModeT, GLint wrapModeR)
    :
    mLayers(layers),
    GLTexture(size, bindingPoint, minFilter, magFilter, wrapModeS, wrapModeT)
    {
        glTexParameteri(bindingPoint, GL_TEXTURE_WRAP_R, wrapModeR);
    }
    
    GLLayeredTexture::~GLLayeredTexture() { }
    
#pragma mark - Getters
    
    GLsizei GLLayeredTexture::layersCount() const {
        return mLayers;
    }
    
}
