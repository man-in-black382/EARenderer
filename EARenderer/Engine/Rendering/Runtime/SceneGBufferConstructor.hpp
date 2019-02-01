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
#include "GPUResourceController.hpp"

namespace EARenderer {

    class SceneGBufferConstructor {
    private:
        const Scene *mScene;
        const SharedResourceStorage *mResourceStorage;
        const GPUResourceController *mGPUResourceController;

        RenderingSettings mSettings;

        GLFramebuffer mFramebuffer;
        GLDepthRenderbuffer mDepthRenderbuffer;
        GLSLGBuffer mGBufferShader;
        GLSLHiZBuffer mHiZBufferShader;

        std::unique_ptr<SceneGBuffer> mGBuffer;

        void generateGBuffer();

        void renderMeshInstance(const MeshInstance &instance, const Transformation *baseTransform = nullptr);

        void generateHiZBuffer();

    public:
        SceneGBufferConstructor(
                const Scene *scene,
                const SharedResourceStorage *resourceStorage,
                const GPUResourceController *gpuResourceController,
                const RenderingSettings &settings
        );

        const SceneGBuffer *GBuffer() const;

        void setRenderingSettings(const RenderingSettings &settings);

        void render();
    };

}

#endif /* SceneGBufferRenderer_hpp */
