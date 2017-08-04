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
        glUniformMatrix4fv(uniformLocation("uProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    void GLSLSkybox::setProjectionMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformLocation("uViewMatrix"), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    void GLSLSkybox::setCubemap(const GLTextureCubemap& cubemap) {
        setUniformTexture("uCubeMapTexture", cubemap);
    }
    
    void GLSLSkybox::setCubemap(const GLDepthTextureCubemap& cubemap) {
        setUniformTexture("uCubeMapTexture", cubemap);
    }
    
}
