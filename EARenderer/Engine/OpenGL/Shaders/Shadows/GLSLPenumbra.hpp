//
//  GLSLPenumbra.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 23.11.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLPenumbra_hpp
#define GLSLPenumbra_hpp

#include "GLProgram.hpp"
#include "FrustumCascades.hpp"
#include "SceneGBuffer.hpp"
#include "Camera.hpp"

namespace EARenderer {
    
    class GLSLPenumbra: public GLProgram {
    public:
        GLSLPenumbra();
        
        void setCamera(const Camera& camera);
        void setGBuffer(const SceneGBuffer& GBuffer);
        void setFrustumCascades(const FrustumCascades& cascades);
        void setDirectionalShadowMapArray(const GLDepthTexture2DArray& array, const GLSampler& bilinearSampler);
    };
    
}

#endif /* GLSLPenumbra_hpp */
