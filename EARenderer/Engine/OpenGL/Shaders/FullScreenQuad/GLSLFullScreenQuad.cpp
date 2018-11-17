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

    void GLSLFullScreenQuad::setTexture(const GLTexture3D& texture, float depth) {
        setUniformTexture(uint32_constant<ctcrc32("uTexture3D")>, texture);
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uDepth")>).location(), depth);
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uShouldSample3DTexture")>).location(), GL_TRUE);
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uShouldSampleArray")>).location(), GL_FALSE);
    }

    void GLSLFullScreenQuad::setApplyToneMapping(bool toneMap) {
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uShouldApplyToneMapping")>).location(), toneMap);
    }
    
}
