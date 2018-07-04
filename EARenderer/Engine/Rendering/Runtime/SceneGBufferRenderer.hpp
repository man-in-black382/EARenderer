//
//  SceneGBufferRenderer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SceneGBufferRenderer_hpp
#define SceneGBufferRenderer_hpp

#include "Scene.hpp"
#include "GLFramebuffer.hpp"
#include "GLDepthRenderbuffer.hpp"
#include "GLHDRTexture2DArray.hpp"
#include "GLSLGBuffer.hpp"
#include "GLSLFullScreenQuad.hpp"
#include "RenderingSettings.hpp"
#include "SceneGBuffer.hpp"

#include <memory>

namespace EARenderer {

    class SceneGBufferRenderer {
    private:
        const Scene *mScene;

        GLFramebuffer mFramebuffer;
        GLDepthRenderbuffer mDepthRenderbuffer;
        GLSLGBuffer mGBufferShader;
        GLSLFullScreenQuad mFSQuadShader;

//        std::shared_ptr<GLHDRTexture2DArray> mGBuffer;
        std::shared_ptr<SceneGBuffer> mGBuffer;

    public:
        SceneGBufferRenderer(const Scene* scene, const RenderingSettings& settings);

        std::shared_ptr<const SceneGBuffer> GBuffer() const;

        void render();
    };

}

#endif /* SceneGBufferRenderer_hpp */
