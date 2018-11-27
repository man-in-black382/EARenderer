//
//  GLSLPenumbra.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 23.11.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLPenumbra.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLSLPenumbra::GLSLPenumbra()
    :
    GLProgram("FullScreenQuad.vert", "Penumbra.frag", "")
    { }
    
#pragma mark - Setters
    
    void GLSLPenumbra::setCamera(const Camera& camera) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraViewInverse")>).location(), 1, GL_FALSE,
                           glm::value_ptr(camera.inverseViewMatrix()));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraProjectionInverse")>).location(), 1, GL_FALSE,
                           glm::value_ptr(camera.inverseProjectionMatrix()));
    }
    
    void GLSLPenumbra::setGBuffer(const SceneGBuffer& GBuffer) {
        setUniformTexture(uint32_constant<ctcrc32("uGBufferHiZBuffer")>, *GBuffer.HiZBuffer);
    }
    
    void GLSLPenumbra::setFrustumCascades(const FrustumCascades& cascades) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uLightSpaceMatrices[0]")>).location(),
                           static_cast<GLsizei>(cascades.lightViewProjections.size()), GL_FALSE,
                           reinterpret_cast<const GLfloat *>(cascades.lightViewProjections.data()));
        
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uDepthSplitsAxis")>).location(), static_cast<GLint>(cascades.splitAxis));
        
        glUniform1fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDepthSplits[0]")>).location(),
                     static_cast<GLsizei>(cascades.splits.size()),
                     reinterpret_cast<const GLfloat *>(cascades.splits.data()));
        
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCSMSplitSpaceMat")>).location(), 1, GL_FALSE, glm::value_ptr(cascades.splitSpaceMatrix));
    }
    
    void GLSLPenumbra::setDirectionalShadowMapArray(const GLDepthTexture2DArray& array, const GLSampler& bilinearSampler) {
        setUniformTexture(uint32_constant<ctcrc32("uDirectionalShadowMapsBilinearSampler")>, array, &bilinearSampler);
    }
    
}
