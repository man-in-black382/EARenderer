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
    
    GLLayeredTexture::GLLayeredTexture(GLsizei layers, GLenum bindingPoint, GLint minFilter, GLint magFilter, GLint wrapModeS, GLint wrapModeT)
    :
    mLayers(layers),
    GLTexture(bindingPoint, minFilter, magFilter, wrapModeS, wrapModeT)
    { }
    
    GLLayeredTexture::GLLayeredTexture(const Size2D& size, GLsizei layers, GLenum bindingPoint, GLint minFilter, GLint magFilter, GLint wrapModeS, GLint wrapModeT)
    :
    mLayers(layers),
    GLTexture(size, bindingPoint, minFilter, magFilter, wrapModeS, wrapModeT)
    { }
    
    GLLayeredTexture::~GLLayeredTexture() { }
    
#pragma mark - Getters
    
    GLsizei GLLayeredTexture::layersCount() const {
        return mLayers;
    }
    
}
