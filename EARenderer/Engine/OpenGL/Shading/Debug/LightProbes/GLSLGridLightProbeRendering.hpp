//
//  GLSLLightProbeRendering.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 4/2/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLLightProbeRendering_hpp
#define GLSLLightProbeRendering_hpp

#include "GLProgram.hpp"
#include "GLHDRTexture3D.hpp"
#include "Camera.hpp"

namespace EARenderer {

    class GLSLGridLightProbeRendering: public GLProgram {
    public:
        GLSLGridLightProbeRendering();

        void setCamera(const Camera& camera);
        void setGridProbesSHTextures(const std::array<GLHDRTexture3D, 7>& textures);
        void setWorldBoundingBox(const AxisAlignedBox3D& box);
        void setProbesGridResolution(const glm::ivec3& resolution);
        void setSphereRadius(float radius);
    };

}

#endif /* GLSLLightProbeRendering_hpp */
