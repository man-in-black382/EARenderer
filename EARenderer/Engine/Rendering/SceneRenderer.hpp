//
//  Renderer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include <unordered_set>

#include "DefaultRenderComponentsProviding.hpp"
#include "Scene.hpp"
#include "GLSLProgramFacility.hpp"
#include "GLFramebuffer.hpp"
#include "GLDepthTexture2D.hpp"

#include "Ray3D.hpp"

namespace EARenderer {
    
    class SceneRenderer {
    private:
        Scene *mScene = nullptr;
        GLSLProgramFacility *mProgramFacility = nullptr;
        DefaultRenderComponentsProviding *mDefaultRenderComponentsProvider = nullptr;
        
        GLDepthTexture2D mDepthTexture;
        GLFramebuffer mDepthFramebuffer;
        
        std::unordered_set<ID> mMeshesToHighlight;
        
    public:
        SceneRenderer(Scene* scene, GLSLProgramFacility* facility);
        
        void setDefaultRenderComponentsProvider(DefaultRenderComponentsProviding *provider);
        void setMeshHiglightEnabled(bool enabled, ID meshID);
        void disableMeshesHighlight();
        
        bool raySelectsMesh(const Ray3D& ray, ID& meshID);
        
        void render();
    };
    
}

#endif /* Renderer_hpp */
