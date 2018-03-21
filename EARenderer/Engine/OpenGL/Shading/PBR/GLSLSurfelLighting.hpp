//
//  GLSLSurfelLighting.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLSurfelLighting_hpp
#define GLSLSurfelLighting_hpp

#include "GLProgram.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "DirectionalLight.hpp"
#include "PointLight.hpp"

namespace EARenderer {

    class GLSLSurfelLighting: public GLProgram {
    public:
        GLSLSurfelLighting();

        void setLight(const PointLight& light);
        void setLight(const DirectionalLight& light);
        void setShadowMapsUniforms(const FrustumCascades& cascades, const GLDepthTexture2DArray& shadowMaps);
    };

}

#endif /* GLSLSurfelLighting_hpp */
