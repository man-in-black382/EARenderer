//
//  GLSLDeferredCookTorrance.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLDeferredCookTorrance_hpp
#define GLSLDeferredCookTorrance_hpp

#include "GLProgram.hpp"
#include "PBRMaterial.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "GLTexture2D.hpp"
#include "GLHDRTexture2D.hpp"
#include "GLHDRTextureCubemap.hpp"
#include "GLHDRTexture2DArray.hpp"
#include "GLHDRTexture3D.hpp"
#include "GLLDRTexture3D.hpp"
#include "GLLDRTextureCubemap.hpp"
#include "PointLight.hpp"
#include "DirectionalLight.hpp"
#include "Camera.hpp"
#include "FrustumCascades.hpp"
#include "LightProbe.hpp"
#include "AxisAlignedBox3D.hpp"
#include "RenderingSettings.hpp"
#include "SceneGBuffer.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <array>

namespace EARenderer {

    class GLSLDeferredCookTorrance: public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLDeferredCookTorrance();

        void setCamera(const Camera& camera);
        void setLight(const PointLight& light);
        void setLight(const DirectionalLight& light);

        void setPreviousFrame(const GLFloatTexture2D& previousFrame);
        void setGBuffer(const SceneGBuffer& GBuffer);
        void setFrustumCascades(const FrustumCascades& cascades);
        void setExponentialShadowMap(const GLFloatTexture2D& map);
        void setGridProbesSHTextures(const std::array<GLLDRTexture3D, 4>& textures);
        void setWorldBoundingBox(const AxisAlignedBox3D& box);
        void setProbePositions(const GLFloat3BufferTexture<glm::vec3>& positions);
        void setSettings(const RenderingSettings& settings);
    };

}

#endif /* GLSLDeferredCookTorrance_hpp */
