//
//  GLFSQuadTextureRenderingProgram.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLFSQuadTextureRenderingProgram_hpp
#define GLFSQuadTextureRenderingProgram_hpp

#include "GLProgram.hpp"
#include "GLTexture2D.hpp"
#include "GLDepthTexture2D.hpp"

namespace EARenderer {
    
    class GLSLFullScreenQuad: public GLProgram {
    public:
        using GLProgram::GLProgram;
        
        GLSLFullScreenQuad();
        
        void setTexture(const GLTexture2D& texture);
        void setTexture(const GLDepthTexture2D& texture);
    };
    
}

#endif /* GLFSQuadTextureRenderingProgram_hpp */
