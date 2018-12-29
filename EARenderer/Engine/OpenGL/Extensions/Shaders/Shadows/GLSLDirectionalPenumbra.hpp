//
//  GLSLDirectionalPenumbra.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 23.11.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLDirectionalPenumbra_hpp
#define GLSLDirectionalPenumbra_hpp

#include "GLProgram.hpp"
#include "FrustumCascades.hpp"
#include "SceneGBuffer.hpp"
#include "Camera.hpp"
#include "DirectionalLight.hpp"

namespace EARenderer {

    class GLSLDirectionalPenumbra : public GLProgram {
    public:
        GLSLDirectionalPenumbra();

        void setCamera(const Camera &camera);

        void setGBuffer(const SceneGBuffer &GBuffer);

        void setFrustumCascades(const FrustumCascades &cascades);

        void setLight(const DirectionalLight& light);

        void setDirectionalShadowMapArray(const GLDepthTexture2DArray &array, const GLSampler &bilinearSampler);
    };

}

#endif /* GLSLPenumbra_hpp */
