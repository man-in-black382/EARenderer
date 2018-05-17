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
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uProjectionMatrix")).location(), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    void GLSLSkybox::setProjectionMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uViewMatrix")).location(), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    void GLSLSkybox::setCubemap(const GLTextureCubemap& cubemap) {
        setUniformTexture(ctcrc32("uCubeMapTexture"), cubemap);
        glUniform1i(uniformByNameCRC32(ctcrc32("uIsHDR")).location(), GL_FALSE);
        glUniform1i(uniformByNameCRC32(ctcrc32("uIsCube")).location(), GL_TRUE);
    }
    
    void GLSLSkybox::setCubemap(const GLDepthTextureCubemap& cubemap) {
        setUniformTexture(ctcrc32("uCubeMapTexture"), cubemap);
        glUniform1i(uniformByNameCRC32(ctcrc32("uIsHDR")).location(), GL_FALSE);
        glUniform1i(uniformByNameCRC32(ctcrc32("uIsCube")).location(), GL_TRUE);
    }
    
    void GLSLSkybox::setCubemap(const GLHDRTextureCubemap& cubemap) {
        setUniformTexture(ctcrc32("uCubeMapTexture"), cubemap);
        glUniform1i(uniformByNameCRC32(ctcrc32("uIsHDR")).location(), GL_TRUE);
        glUniform1i(uniformByNameCRC32(ctcrc32("uIsCube")).location(), GL_TRUE);
    }

    void GLSLSkybox::setCubemap(const GLLDRTextureCubemap& cubemap) {
        setUniformTexture(ctcrc32("uUICubeMapTexture"), cubemap);
        glUniform1i(uniformByNameCRC32(ctcrc32("uIsHDR")).location(), GL_FALSE);
        glUniform1i(uniformByNameCRC32(ctcrc32("uIsCube")).location(), GL_TRUE);
        glUniform1i(uniformByNameCRC32(ctcrc32("uIsIntegerCube")).location(), GL_TRUE);
    }
    
    void GLSLSkybox::setEquirectangularMap(const GLHDRTexture2D& equireqMap) {
        setUniformTexture(ctcrc32("uEquirectangularMap"), equireqMap);
        glUniform1i(uniformByNameCRC32(ctcrc32("uIsHDR")).location(), GL_TRUE);
        glUniform1i(uniformByNameCRC32(ctcrc32("uIsCube")).location(), GL_FALSE);
    }

}
