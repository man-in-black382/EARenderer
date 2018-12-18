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
#include "ResourcePool.hpp"

namespace EARenderer {

    class TriangleRenderer {
    private:
        GLSLTriangleRendering mTriangleRenderingShader;
        Scene *mScene;
        ResourcePool *mResourcePool;

    public:
        TriangleRenderer(Scene *scene, ResourcePool *resourcePool);

        void render();
    };

}

#endif /* TriangleRenderer_hpp */
