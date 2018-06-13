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

    class GLSLDirectionalShadowMap: public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLDirectionalShadowMap();

        void setModelMatrix(const glm::mat4& matrix);
        void setViewProjectionMatrix(const glm::mat4& matrix);
    };

}

#endif /* GLSLDirectionalShadowMap_hpp */
