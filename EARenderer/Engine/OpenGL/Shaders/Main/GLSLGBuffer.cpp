//
//  GLSLGBuffer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLGBuffer.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLGBuffer::GLSLGBuffer()
    :
    GLProgram("GBuffer.vert", "GBuffer.frag", "")
    { }

#pragma mark - Setters

    void GLSLGBuffer::setCamera(const Camera& camera) {
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraPosition")>).location(), 1, glm::value_ptr(camera.position()));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraViewMat")>).location(), 1, GL_FALSE, glm::value_ptr(camera.viewMatrix()));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraProjectionMat")>).location(), 1, GL_FALSE, glm::value_ptr(camera.projectionMatrix()));
    }

    void GLSLGBuffer::setModelMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uModelMat")>).location(), 1, GL_FALSE, glm::value_ptr(matrix));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uNormalMat")>).location(), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(matrix))));
    }
    
    void GLSLGBuffer::setMaterial(const PBRMaterial& material) {
        if (material.albedoMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.albedoMap")>, *material.albedoMap()); }
        if (material.normalMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.normalMap")>, *material.normalMap()); }
        if (material.metallicMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.metallicMap")>, *material.metallicMap()); }
        if (material.roughnessMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.roughnessMap")>, *material.roughnessMap()); }
        if (material.ambientOcclusionMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.AOMap")>, *material.ambientOcclusionMap()); }
        if (material.displacementMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.displacementMap")>, *material.displacementMap()); }
    }

}
