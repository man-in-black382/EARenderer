//
//  GLSLSpecularRadianceConvolution.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 12.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLSpecularRadianceConvolution.hpp"

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLSpecularRadianceConvolution::GLSLSpecularRadianceConvolution()
            : GLSLCubemapRendering("SpecularRadianceConvolution.frag") {}

#pragma mark - Setters

    void GLSLSpecularRadianceConvolution::setEnvironmentRadianceMap(const GLFloatTextureCubemap<GLTexture::Float::RGB16F> &map) {
        setUniformTexture(ctcrc32("uEnvironmentMap"), map);
    }

    void GLSLSpecularRadianceConvolution::setRoughness(float roughness) {
        glUniform1f(uniformByNameCRC32(ctcrc32("uRoughness")).location(), roughness);
    }

}
