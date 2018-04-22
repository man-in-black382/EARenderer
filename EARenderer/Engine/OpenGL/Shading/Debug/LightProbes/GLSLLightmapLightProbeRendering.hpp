//
//  GLSLLightProbeRendering.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 4/2/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLLightmapLightProbeRendering_hpp
#define GLSLLightmapLightProbeRendering_hpp

#include "GLProgram.hpp"
#include "GLHDRTexture2DArray.hpp"
#include "Camera.hpp"

namespace EARenderer {

    class GLSLLightmapLightProbeRendering: public GLProgram {
    public:
        GLSLLightmapLightProbeRendering();

        void setCamera(const Camera& camera);
        void setLightmapProbesSHTextures(const GLHDRTexture2DArray& textures);
        void setSphereRadius(float radius);
    };

}

#endif /* GLSLLightProbeRendering_hpp */
