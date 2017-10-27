//
//  GLSkyboxProgram.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 22.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLSkybox.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLSLSkybox::GLSLSkybox()
    :
    GLProgram("Skybox.vert", "Skybox.frag", "")
    { }
    
#pragma mark - Setters
    
    void GLSLSkybox::setViewMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformByName("uProjectionMatrix").location(), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    void GLSLSkybox::setProjectionMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformByName("uViewMatrix").location(), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    void GLSLSkybox::setCubemap(const GLTextureCubemap& cubemap) {
        setUniformTexture("uCubeMapTexture", cubemap);
        glUniform1i(uniformByName("uIsHDR").location(), GL_FALSE);
        glUniform1i(uniformByName("uIsCube").location(), GL_TRUE);
    }
    
    void GLSLSkybox::setCubemap(const GLDepthTextureCubemap& cubemap) {
        setUniformTexture("uCubeMapTexture", cubemap);
        glUniform1i(uniformByName("uIsHDR").location(), GL_FALSE);
        glUniform1i(uniformByName("uIsCube").location(), GL_TRUE);
    }
    
    void GLSLSkybox::setCubemap(const GLHDRTextureCubemap& cubemap) {
        setUniformTexture("uCubeMapTexture", cubemap);
        glUniform1i(uniformByName("uIsHDR").location(), GL_TRUE);
        glUniform1i(uniformByName("uIsCube").location(), GL_TRUE);
    }
    
    void GLSLSkybox::setEquirectangularMap(const GLHDRTexture2D& equireqMap) {
        setUniformTexture("uEquirectangularMap", equireqMap);
        glUniform1i(uniformByName("uIsHDR").location(), GL_TRUE);
        glUniform1i(uniformByName("uIsCube").location(), GL_FALSE);
    }
    
}
