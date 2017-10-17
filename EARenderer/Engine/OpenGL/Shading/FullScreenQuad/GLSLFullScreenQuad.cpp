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
        glUniform1i(uniformByName("uShouldSampleArray").location(), GL_FALSE);
    }
    
    void GLSLFullScreenQuad::setTexture(const GLHDRTexture2D& texture) {
        setUniformTexture("uTexture", texture);
        glUniform1i(uniformByName("uShouldSampleArray").location(), GL_FALSE);
    }
    
    void GLSLFullScreenQuad::setTexture(const GLDepthTexture2D& texture) {
        setUniformTexture("uTexture", texture);
        glUniform1i(uniformByName("uShouldSampleArray").location(), GL_FALSE);
    }
    
    void GLSLFullScreenQuad::setTexture(const GLTexture2DArray& texture, uint8_t index) {
        setUniformTexture("uTextureArray", texture);
        glUniform1i(uniformByName("uIndex").location(), index);
        glUniform1i(uniformByName("uShouldSampleArray").location(), GL_TRUE);
    }
    
    void GLSLFullScreenQuad::setTexture(const GLDepthTexture2DArray& texture, uint8_t index) {
        setUniformTexture("uTextureArray", texture);
        glUniform1i(uniformByName("uIndex").location(), index);
        glUniform1i(uniformByName("uShouldSampleArray").location(), GL_TRUE);
    }
    
}
