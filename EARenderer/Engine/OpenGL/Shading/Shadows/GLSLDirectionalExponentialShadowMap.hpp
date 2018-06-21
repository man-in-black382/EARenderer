//
//  GLSLDirectionalShadowMap.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/13/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLDirectionalShadowMap_hpp
#define GLSLDirectionalShadowMap_hpp

#include "GLProgram.hpp"
#include "FrustumCascades.hpp"
#include "Camera.hpp"

#include <glm/mat4x4.hpp>

namespace EARenderer {

    class GLSLDirectionalExponentialShadowMap: public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLDirectionalExponentialShadowMap();

        void setModelMatrix(const glm::mat4& modelMatrix);
        void setCamera(const Camera& camera);
        void setESMFactor(float factor);
        void setFrustumCascades(const FrustumCascades& cascades);
    };

}

#endif /* GLSLDirectionalShadowMap_hpp */
