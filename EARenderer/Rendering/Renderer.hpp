//
//  Renderer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include <map>
#include <OpenGL/gl.h>
#include "Scene.hpp"
#include "GLProgram.hpp"
#include "GLVertexArray.hpp"

namespace EARenderer {
    
    class Renderer {
    private:
        Scene *mScene;
        GLProgram *mProgram;
        std::map<ID, GLVertexArray<Vertex1P1N1UV> *> mVAOs;
        
    public:
        Renderer(Scene* scene, GLProgram* program);
        void render();
    };
    
}

#endif /* Renderer_hpp */
