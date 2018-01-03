//
//  SurfelsRenderer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.12.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef SurfelRenderer_hpp
#define SurfelRenderer_hpp

#include "Scene.hpp"
#include "GLSLSurfelRendering.hpp"
#include "Surfel.hpp"

namespace EARenderer {
    
    class SurfelRenderer {
    private:
        Scene *mScene;
        ResourcePool *mResourcePool;
        GLVertexArray<Surfel> mVAO;
        std::vector<Surfel> mSurfels;
        GLSLSurfelRendering mSurfelRenderingShader;
        
    public:
        SurfelRenderer(Scene* scene, ResourcePool* resourcePool);
        
        void render();
    };
    
}

#endif /* SurfelRenderer_hpp */
