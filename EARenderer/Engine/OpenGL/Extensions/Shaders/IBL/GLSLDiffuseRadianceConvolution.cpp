//
// Created by Pavlo Muratov on 2019-01-23.
// Copyright (c) 2019 MPO. All rights reserved.
//

#include "GLSLDiffuseRadianceConvolution.hpp"

namespace EARenderer {

    GLSLDiffuseRadianceConvolution::GLSLDiffuseRadianceConvolution()
            : GLSLCubemapRendering("DiffuseRadianceConvolution.frag") {}

    void GLSLDiffuseRadianceConvolution::setEnvironmentRadianceMap(const GLFloatTextureCubemap<GLTexture::Float::RGB16F> &map) {
        setUniformTexture(ctcrc32("uEnvironmentMap"), map);
    }

}