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

#include "Ray.hpp"

namespace EARenderer {
    
    class Renderer {
    private:
        GLSLProgramFacility *mProgramFacility;
        GLDepthTexture2D mDepthTexture;
        GLFramebuffer mDepthFramebuffer;
        DefaultRenderComponentsProviding *mDefaultRenderComponentsProvider;
        
    public:
        Renderer(GLSLProgramFacility *facility);
        
        void setDefaultRenderComponentsProvider(DefaultRenderComponentsProviding *provider);
        void render(Scene *scene);
    };
    
}

#endif /* Renderer_hpp */
