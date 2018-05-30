//
//  GLSLSurfelLighting.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLSurfelLighting_hpp
#define GLSLSurfelLighting_hpp

#include "GLProgram.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "GLHDRTexture2DArray.hpp"
#include "DirectionalLight.hpp"
#include "PointLight.hpp"
#include "GLHDRTexture3D.hpp"
#include "GLBufferTexture.hpp"

namespace EARenderer {

    class GLSLSurfelLighting: public GLProgram {
    public:
        GLSLSurfelLighting();

        void setLight(const PointLight& light);
        void setLight(const DirectionalLight& light);
        void setShadowMapsUniforms(const FrustumCascades& cascades, const GLDepthTexture2DArray& shadowMaps);
        void setSurfelsGBuffer(const GLHDRTexture2DArray& gBuffer);
        void setGridProbesSHTextures(const std::array<GLHDRTexture3D, 7>& textures);
        void setWorldBoundingBox(const AxisAlignedBox3D& box);
        void setProbePositions(const GLFloat3BufferTexture<glm::vec3>& positions);
    };

}

#endif /* GLSLSurfelLighting_hpp */
