//
// Created by Pavlo Muratov on 2019-01-23.
// Copyright (c) 2019 MPO. All rights reserved.
//

#ifndef EARENDERER_GLSLDIFFUSERADIANCECONVOLUTION_HPP
#define EARENDERER_GLSLDIFFUSERADIANCECONVOLUTION_HPP

#include "GLSLCubemapRendering.hpp"
#include "GLTextureCubemap.hpp"

namespace EARenderer {

    class GLSLDiffuseRadianceConvolution: public GLSLCubemapRendering {
    public:
        GLSLDiffuseRadianceConvolution();

        void setEnvironmentRadianceMap(const GLFloatTextureCubemap<GLTexture::Float::RGB16F> &map);
    };

}

#endif //EARENDERER_GLSLDIFFUSERADIANCECONVOLUTION_HPP
