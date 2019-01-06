//
//  TriangleRenderer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.12.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef TriangleRenderer_hpp
#define TriangleRenderer_hpp

#include "GLSLTriangleRendering.hpp"
#include "Scene.hpp"
#include "SharedResourceStorage.hpp"
#include "GPUResourceController.hpp"

namespace EARenderer {

    class TriangleRenderer {
    private:
        GLSLTriangleRendering mTriangleRenderingShader;
        const Scene *mScene;
        const SharedResourceStorage *mResourceStorage;
        const GPUResourceController *mGPUResourceController;

    public:
        TriangleRenderer(const Scene *scene, const SharedResourceStorage *resourceStorage, const GPUResourceController *gpuResourceController);

        void render();
    };

}

#endif /* TriangleRenderer_hpp */
