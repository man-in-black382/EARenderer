//
//  GLSLDeferredCookTorrance.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLDirectLightEvaluation_hpp
#define GLSLDirectLightEvaluation_hpp

#include "GLProgram.hpp"
#include "CookTorranceMaterial.hpp"
#include "GLTexture2D.hpp"
#include "GLTexture2DArray.hpp"
#include "GLHDRTexture3D.hpp"
#include "GLLDRTexture3D.hpp"
#include "GLTextureCubemapArray.hpp"
#include "PointLight.hpp"
#include "DirectionalLight.hpp"
#include "Camera.hpp"
#include "FrustumCascades.hpp"
#include "AxisAlignedBox3D.hpp"
#include "RenderingSettings.hpp"
#include "SceneGBuffer.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <array>

namespace EARenderer {

    class GLSLDirectLightEvaluation : public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLDirectLightEvaluation();

        void setCamera(const Camera &camera);

        void setLightType(LightType type);

        void setLight(const DirectionalLight &light);

        void setGBuffer(const SceneGBuffer &GBuffer);

        void setFrustumCascades(const FrustumCascades &cascades);

        void setDirectionalShadowMapArray(const GLDepthTexture2DArray &array);

        void setOmnidirectionalShadowCubemap(const GLDepthTextureCubemap &cubemap);

        void setPenumbra(const GLFloatTexture2D<GLTexture::Float::R16F> &penumbra);

        void setSettings(const RenderingSettings &settings);
    };

}

#endif /* GLSLDirectLightEvaluation_hpp */
