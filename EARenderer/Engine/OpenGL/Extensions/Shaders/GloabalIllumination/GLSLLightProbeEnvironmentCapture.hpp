//
//  GLSLLightProbeEnvironmentCapture.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/4/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLSLLightProbeEnvironmentCapture_hpp
#define GLSLLightProbeEnvironmentCapture_hpp

#include "GLProgram.hpp"

#include "CookTorranceMaterial.hpp"
#include "PointLight.hpp"
#include "DirectionalLight.hpp"

namespace EARenderer {

    class GLSLLightProbeEnvironmentCapture : public GLProgram {
    public:
        GLSLLightProbeEnvironmentCapture();

        void setModelMatrix(const glm::mat4 &modelMatrix);

//        void setLightProbe(const LightProbe& probe);
        void setLight(const PointLight &light);

        void setLight(const DirectionalLight &light);

        void setMaterial(const CookTorranceMaterial &material);
    };

}

#endif /* GLSLLightProbeEnvironmentCapture_hpp */
