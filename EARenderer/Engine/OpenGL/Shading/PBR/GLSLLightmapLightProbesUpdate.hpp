//
//  GLSLGridLightProbesUpdate.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 3/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLLightmapLightProbesUpdate_hpp
#define GLSLLightmapLightProbesUpdate_hpp

#include "GLProgram.hpp"
#include "GLTextureBuffer.hpp"
#include "GLLDRTexture2D.hpp"
#include "GLHDRTexture2D.hpp"
#include "SphericalHarmonics.hpp"

namespace EARenderer {

    class GLSLLightmapLightProbesUpdate: public GLProgram {
    public:
        GLSLLightmapLightProbesUpdate();

        void setSurfelClustersLuminaceMap(const GLHDRTexture2D& luminanceMap);
        void setLightmapProbeIndicesMap(const GLLDRTexture2D& indicesMap);
        void setProjectionClusterSphericalHarmonics(const GLFloat3BufferTexture<SphericalHarmonics>& SH);
        void setProjectionClusterIndices(const GLUIntegerBufferTexture<uint32_t>& indices);
        void setProbeProjectionsMetadata(const GLUIntegerBufferTexture<uint32_t>& metadata);
    };

}

#endif /* GLSLLightmapLightProbesUpdate_hpp */
