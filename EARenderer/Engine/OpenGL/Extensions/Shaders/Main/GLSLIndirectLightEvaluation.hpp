//
//  IndirectLightEvaluation.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 04.11.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef IndirectLightEvaluation_hpp
#define IndirectLightEvaluation_hpp

#include "GLProgram.hpp"
#include "Camera.hpp"
#include "SceneGBuffer.hpp"
#include "GLLDRTexture3D.hpp"
#include "RenderingSettings.hpp"
#include "GLTexture2D.hpp"
#include "ImageBasedLightProbe.hpp"

namespace EARenderer {

    class GLSLIndirectLightEvaluation : public GLProgram {
    public:
        GLSLIndirectLightEvaluation();

        void setCamera(const Camera &camera);

        void setGBuffer(const SceneGBuffer &GBuffer);

        void setGridProbesSHTextures(const std::array<GLLDRTexture3D, 4> &textures);

        void setWorldBoundingBox(const AxisAlignedBox3D &box);

        void setProbePositions(const GLFloatBufferTexture<GLTexture::Float::RGB32F, glm::vec3> &positions);

        void setSettings(const RenderingSettings& settings);
    };

}


#endif /* IndirectLightEvaluation_hpp */
