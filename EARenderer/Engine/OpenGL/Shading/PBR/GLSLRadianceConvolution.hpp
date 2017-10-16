//
//  GLSLRadianceConvolution.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 12.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLSLRadianceConvolution_hpp
#define GLSLRadianceConvolution_hpp

#include "GLSLCubemapRendering.hpp"
#include "GLHDRTextureCubemap.hpp"

namespace EARenderer {
    
    class GLSLRadianceConvolution: public GLSLCubemapRendering {
    public:
        GLSLRadianceConvolution();
        
        void setEnvironmentRadianceMap(const GLHDRTextureCubemap& map);
        void setRoughness(float roughness);
    };
    
}

#endif /* GLSLRadianceConvolution_hpp */
