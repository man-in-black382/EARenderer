//
//  GLFSQuadTextureRenderingProgram.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLFullScreenQuad.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLSLFullScreenQuad::GLSLFullScreenQuad()
    :
    GLProgram("FullScreenQuad.vert", "FullScreenQuad.frag", "")
    { }
    
#pragma mark - Setters
    
    void GLSLFullScreenQuad::setTexture(const GLTexture2D& texture) {
        setUniformTexture("uTexture", texture);
    }
    
    void GLSLFullScreenQuad::setTexture(const GLDepthTexture2D& texture) {
        setUniformTexture("uTexture", texture);
    }
    
    void GLSLFullScreenQuad::setTexture(const GLTexture2DArray& texture) {
        setUniformTexture("uTextureArray", texture);
    }
    
    void GLSLFullScreenQuad::setTexture(const GLDepthTexture2DArray& texture, uint8_t index) {
        setUniformTexture("uTextureArray", texture);
        glUniform1i(uniformByName("uIndex").location(), index);
    }
    
}
