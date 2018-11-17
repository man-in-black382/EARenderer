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

#include <glm/mat4x4.hpp>

namespace EARenderer {
    
    class GLSLDirectionalShadowMap: public GLProgram {
    public:        
        GLSLDirectionalShadowMap();
        
        void setModelMatrix(const glm::mat4& modelMatrix);
        void setCascades(const FrustumCascades& cascades);
    };
    
}

#endif /* GLSLDirectionalShadowMap_hpp */
