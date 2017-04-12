//
//  Renderer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include "Scene.hpp"
#include "GLSLProgramFacility.hpp"

namespace EARenderer {
    
    class Renderer {
    private:
        GLSLProgramFacility *mProgramFacility;
        
    public:
        Renderer(GLSLProgramFacility *facility);
        void render(Scene *scene);
    };
    
}

#endif /* Renderer_hpp */
