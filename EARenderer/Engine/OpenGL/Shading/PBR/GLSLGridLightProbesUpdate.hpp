//
//  GLSLGridLightProbesUpdate.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 3/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLGridLightProbesUpdate_hpp
#define GLSLGridLightProbesUpdate_hpp

#include "GLProgram.hpp"
#include "GLTextureBuffer.hpp"
#include "GLHDRTexture2D.hpp"
#include "SphericalHarmonics.hpp"

namespace EARenderer {

    class GLSLGridLightProbesUpdate: public GLProgram {
    public:
        GLSLGridLightProbesUpdate();

        void setProbesPerGridDimensionCount(size_t count);
        void setSurfelClustersLuminaceMap(const GLHDRTexture2D& luminanceMap);
        void setProjectionClusterSphericalHarmonics(const GLFloat3BufferTexture<SphericalHarmonics>& SH);
        void setProjectionClusterIndices(const GLUIntegerBufferTexture<uint32_t>& indices);
        void setProbeProjectionsMetadata(const GLUIntegerBufferTexture<glm::uvec2>& metadata);
    };

}


#endif /* GLSLGridLightProbesUpdate_hpp */
