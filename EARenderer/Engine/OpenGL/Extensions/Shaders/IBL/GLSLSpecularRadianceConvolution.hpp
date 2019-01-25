//
//  GLSLSpecularRadianceConvolution.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 12.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLSLRadianceConvolution_hpp
#define GLSLRadianceConvolution_hpp

#include "GLSLCubemapRendering.hpp"
#include "GLTextureCubemap.hpp"

namespace EARenderer {

    class GLSLSpecularRadianceConvolution : public GLSLCubemapRendering {
    public:
        GLSLSpecularRadianceConvolution();

        void setEnvironmentRadianceMap(const GLFloatTextureCubemap<GLTexture::Float::RGB16F> &map);

        void setRoughness(float roughness);
    };

}

#endif /* GLSLRadianceConvolution_hpp */
