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

#include <glm/mat4x4.hpp>

namespace EARenderer {

    class GLSLDirectionalExponentialShadowMap: public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLDirectionalExponentialShadowMap();

        void setMVPMatrix(const glm::mat4& matrix);
        void setESMFactor(float factor);
    };

}

#endif /* GLSLDirectionalShadowMap_hpp */
