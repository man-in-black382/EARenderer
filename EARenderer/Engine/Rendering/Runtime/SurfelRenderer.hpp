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
#include "SurfelCluster.hpp"

#include <vector>

namespace EARenderer {
    
    class SurfelRenderer {
    private:
        Scene *mScene;
        ResourcePool *mResourcePool;
        
        std::vector<GLVertexArray<Surfel>> mSurfelClusterVAOs;
        std::vector<Color> mSurfelClusterColors;
        GLSLSurfelRendering mSurfelRenderingShader;

    public:
        enum class Mode { Default, Clusters };

        SurfelRenderer(Scene* scene, ResourcePool* resourcePool);
        
        void render(Mode renderingMode, float surfelRadius);
    };
    
}

#endif /* SurfelRenderer_hpp */
