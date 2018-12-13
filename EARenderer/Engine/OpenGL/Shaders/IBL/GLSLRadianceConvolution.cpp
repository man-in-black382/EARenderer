//
//  GLSLRadianceConvolution.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 12.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLRadianceConvolution.hpp"

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLSLRadianceConvolution::GLSLRadianceConvolution()
    :
    GLSLCubemapRendering("RadianceConvolution.frag")
    { }
    
#pragma mark - Setters
    
//    void GLSLRadianceConvolution::setEnvironmentRadianceMap(const GLHDRTextureCubemap& map) {
//        setUniformTexture(uint32_constant<ctcrc32("uEnvironmentMap")>, map);
//        glUniform1f(uniformByNameCRC32(uint32_constant<ctcrc32("uEnvironmentResolution")>).location(), map.size().width);
//    }
    
    void GLSLRadianceConvolution::setRoughness(float roughness) {
        glUniform1f(uniformByNameCRC32(uint32_constant<ctcrc32("uRoughness")>).location(), roughness);
    }
    
    void GLSLRadianceConvolution::setAlgorithm(Algorithm algorithm) {
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uAlgorithm")>).location(), static_cast<GLint>(algorithm));
    }
    
}
