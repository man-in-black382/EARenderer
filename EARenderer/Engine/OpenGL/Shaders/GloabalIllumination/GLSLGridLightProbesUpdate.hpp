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
#include "GLTexture2D.hpp"
#include "SphericalHarmonics.hpp"

namespace EARenderer {

    class GLSLGridLightProbesUpdate : public GLProgram {
    public:
        GLSLGridLightProbesUpdate();

        void setProbesGridResolution(const glm::ivec3 &resolution);

        void setSurfelClustersLuminaceMap(const GLFloatTexture2D<GLTexture::Float::R16F> &luminanceMap);

        void setProjectionClusterSphericalHarmonics(const GLFloat3BufferTexture<SphericalHarmonics> &SH);

        void setSkySphericalHarmonics(const GLFloat3BufferTexture<SphericalHarmonics> &SH);

        void setProjectionClusterIndices(const GLUIntegerBufferTexture<uint32_t> &indices);

        void setProbeProjectionsMetadata(const GLUIntegerBufferTexture<uint32_t> &metadata);

        void setSkyColorSphericalHarmonics(const SphericalHarmonics &skyColorSH);
    };

}


#endif /* GLSLGridLightProbesUpdate_hpp */
