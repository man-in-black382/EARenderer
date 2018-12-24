//
//  GLSLDirectionalShadowMap.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 13.11.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLDirectionalShadowMap_hpp
#define GLSLDirectionalShadowMap_hpp

#include "GLProgram.hpp"
#include "FrustumCascades.hpp"
#include "Camera.hpp"
#include "PointLight.hpp"

#include <glm/mat4x4.hpp>

namespace EARenderer {

    class GLSLShadowMap : public GLProgram {
    public:
        GLSLShadowMap();

        void setModelMatrix(const glm::mat4 &modelMatrix);

        void setViewProjectionMatrices(const std::vector<glm::mat4> &matrices);
    };

}

#endif /* GLSLDirectionalShadowMap_hpp */
