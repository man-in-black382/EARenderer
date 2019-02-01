//
//  GLSLConeTracing.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 7/30/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLConeTracing_hpp
#define GLSLConeTracing_hpp

#include "GLProgram.hpp"
#include "GLTexture2D.hpp"
#include "SceneGBuffer.hpp"
#include "Camera.hpp"
#include "ImageBasedLightProbe.hpp"

namespace EARenderer {

    class GLSLConeTracing : public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLConeTracing();

        void setCamera(const Camera &camera);

        void setGBuffer(const SceneGBuffer &GBuffer);

        void setReflections(const GLFloatTexture2D<GLTexture::Float::RGBA16F> &reflections);

        void setRayHitInfo(const GLFloatTexture2D<GLTexture::Float::RGBA16F> &rayHitInfo);

        void setIBLProbe(const ImageBasedLightProbe& probe);

        void setDebugRoughness(float r);
    };

}

#endif /* GLSLConeTracing_hpp */
