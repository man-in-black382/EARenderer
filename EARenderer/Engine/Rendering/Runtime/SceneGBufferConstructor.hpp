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
#include "GLTexture2DArray.hpp"
#include "GLSLGBuffer.hpp"
#include "GLSLHiZBuffer.hpp"
#include "RenderingSettings.hpp"
#include "SceneGBuffer.hpp"

#include <memory>

namespace EARenderer {

    class SceneGBufferConstructor {
    private:
        const Scene *mScene;

        GLFramebuffer mFramebuffer;
        GLDepthRenderbuffer mDepthRenderbuffer;
        GLSLGBuffer mGBufferShader;
        GLSLHiZBuffer mHiZBufferShader;

        std::shared_ptr<SceneGBuffer> mGBuffer;

        void generateGBuffer();
        void generateHiZBuffer();

    public:
        SceneGBufferConstructor(const Scene* scene, const RenderingSettings& settings);

        std::shared_ptr<const SceneGBuffer> GBuffer() const;

        void render();
    };

}

#endif /* SceneGBufferRenderer_hpp */
