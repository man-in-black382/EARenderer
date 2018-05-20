//
//  GLSLProbeOcclusionRendering.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 20.05.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLProbeOcclusionRendering_hpp
#define GLSLProbeOcclusionRendering_hpp

#include "GLProgram.hpp"
#include "Camera.hpp"
#include "GLHDRTexture2D.hpp"

#include <glm/vec3.hpp>

namespace EARenderer {

    class GLSLProbeOcclusionRendering: public GLProgram {
    public:
        GLSLProbeOcclusionRendering();

        void setCamera(const Camera& camera);
        void setDiffuseProbeOcclusionMapsAtlas(const GLHDRTexture2D& atlas);
    };

}

#endif /* GLSLProbeOcclusionRendering_hpp */
