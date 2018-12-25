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
            : GLProgram("GBuffer.vert", "GBuffer.frag", "") {
    }

#pragma mark - Setters

    void GLSLGBuffer::setCamera(const Camera &camera) {
        glUniform3fv(uniformByNameCRC32(ctcrc32("uCameraPosition")).location(), 1, glm::value_ptr(camera.position()));
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uCameraViewMat")).location(), 1, GL_FALSE, glm::value_ptr(camera.viewMatrix()));
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uCameraProjectionMat")).location(), 1, GL_FALSE, glm::value_ptr(camera.projectionMatrix()));
    }

    void GLSLGBuffer::setModelMatrix(const glm::mat4 &matrix) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uModelMat")).location(), 1, GL_FALSE, glm::value_ptr(matrix));
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uNormalMat")).location(), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(matrix))));
    }

    void GLSLGBuffer::setMaterial(const CookTorranceMaterial &material) {
        if (material.albedoMap()) {setUniformTexture(ctcrc32("uMaterialCookTorrance.albedoMap"), *material.albedoMap());}
        if (material.normalMap()) {setUniformTexture(ctcrc32("uMaterialCookTorrance.normalMap"), *material.normalMap());}
        if (material.metallicMap()) {setUniformTexture(ctcrc32("uMaterialCookTorrance.metallicMap"), *material.metallicMap());}
        if (material.roughnessMap()) {setUniformTexture(ctcrc32("uMaterialCookTorrance.roughnessMap"), *material.roughnessMap());}
        if (material.ambientOcclusionMap()) {setUniformTexture(ctcrc32("uMaterialCookTorrance.AOMap"), *material.ambientOcclusionMap());}
        if (material.displacementMap()) {setUniformTexture(ctcrc32("uMaterialCookTorrance.displacementMap"), *material.displacementMap());}

        glUniform1i(uniformByNameCRC32(ctcrc32("uMaterialType")).location(), std::underlying_type<MaterialType>::type(MaterialType::CookTorrance));
    }

    void GLSLGBuffer::setMaterial(const EmissiveMaterial &material) {
        glUniform3fv(uniformByNameCRC32(ctcrc32("uMaterialEmissive.emission")).location(), 1, glm::value_ptr(material.emissionColor.rgb()));
        glUniform1i(uniformByNameCRC32(ctcrc32("uMaterialType")).location(), std::underlying_type<MaterialType>::type(MaterialType::Emissive));
    }

}
