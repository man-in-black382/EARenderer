//
//  GLSLLightComposer.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 9/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLLightComposer_hpp
#define GLSLLightComposer_hpp

#include "GLProgram.hpp"
#include "Camera.hpp"
#include "SceneGBuffer.hpp"
#include "GLLDRTexture3D.hpp"
#include "RenderingSettings.hpp"
#include "GLTexture2D.hpp"

namespace EARenderer {

    class GLSLLightComposer: public GLProgram {
    public:
        GLSLLightComposer();

        void setCamera(const Camera& camera);
        void setGBuffer(const SceneGBuffer& GBuffer);
        void setGridProbesSHTextures(const std::array<GLLDRTexture3D, 4>& textures);
        void setWorldBoundingBox(const AxisAlignedBox3D& box);
        void setProbePositions(const GLFloat3BufferTexture<glm::vec3>& positions);
        void setLightBuffer(const GLFloatTexture2D<GLTexture::Float::RGBA16F>& lightBuffer);
        void setReflections(const GLFloatTexture2D<GLTexture::Float::RGBA16F>& reflections);
    };

}

#endif /* GLSLLightComposer_hpp */