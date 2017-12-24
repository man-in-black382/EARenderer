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
#include "GLSLGenericGeometry.hpp"

namespace EARenderer {
    
    class SurfelRenderer {
    private:
        Scene *mScene;
        ResourcePool *mResourcePool;
        ID mMeshInstanceID;
        GLVertexArray<glm::vec3> mVAO;
        std::vector<glm::vec3> mPoints;
        GLSLGenericGeometry mGenericGeometryShader;
        
    public:
        SurfelRenderer(Scene* scene, ResourcePool* resourcePool, ID meshInstanceID);
        
        void render();
    };
    
}

#endif /* SurfelRenderer_hpp */
