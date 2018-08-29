//
//  GLSLOmnidirectionalExponentialShadowMap.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 8/29/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLOmnidirectionalExponentialShadowMap_hpp
#define GLSLOmnidirectionalExponentialShadowMap_hpp

#include "GLProgram.hpp"
#include "PointLight.hpp"

#include <glm/mat4x4.hpp>

namespace EARenderer {

    class GLSLOmnidirectionalExponentialShadowMap: public GLProgram {
    public:
        GLSLOmnidirectionalExponentialShadowMap();

        void setModelMatrix(const glm::mat4& matrix);
        void setLight(const PointLight& light);
        void setESMFactor(float factor);
    };

}

#endif /* GLSLOmnidirectionalExponentialShadowMap_hpp */
