//
//  GLSLLightProbeLinksRendering.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 4/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLLightProbeLinksRendering_hpp
#define GLSLLightProbeLinksRendering_hpp

#include "GLProgram.hpp"
#include "GLHDRTexture3D.hpp"
#include "Camera.hpp"

#include <glm/vec3.hpp>

namespace EARenderer {

    class GLSLLightProbeLinksRendering : public GLProgram {
    public:
        GLSLLightProbeLinksRendering();

        void setCamera(const Camera &camera);

        void setWorldBoundingBox(const AxisAlignedBox3D &box);

        void setProjectionClusterIndices(const GLIntegerBufferTexture<GLTexture::Integer::R32UI, uint32_t> &indices);

        void setProbeProjectionsMetadata(const GLIntegerBufferTexture<GLTexture::Integer::R32UI, uint32_t> &metadata);

        void setSurfelClusterCenters(const GLFloatBufferTexture<GLTexture::Float::RGB32F, glm::vec3> &centers);

        void setProbesGridResolution(const glm::ivec3 &resolution);
    };

}


#endif /* GLSLLightProbeLinksRendering_hpp */
