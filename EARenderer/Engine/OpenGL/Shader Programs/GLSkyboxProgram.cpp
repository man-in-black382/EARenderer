//
//  GLSkyboxProgram.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 22.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSkyboxProgram.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {
    
    void GLSkyboxProgram::setViewMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformLocation("uProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    void GLSkyboxProgram::setProjectionMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformLocation("uViewMatrix"), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    void GLSkyboxProgram::setCubemap(const GLTextureCubemap& cubemap) {
        setUniformTexture("uCubeMapTexture", cubemap);
    }
    
}
