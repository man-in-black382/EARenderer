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
#include "GLHDRTexture2D.hpp"
#include "GLDepthTexture2D.hpp"
#include "GLTexture2DArray.hpp"
#include "GLDepthTexture2DArray.hpp"

namespace EARenderer {
    
    class GLSLFullScreenQuad: public GLProgram {
    public:
        using GLProgram::GLProgram;
        
        GLSLFullScreenQuad();
        
        void setTexture(const GLTexture2D& texture);
        void setTexture(const GLHDRTexture2D& texture);
        void setTexture(const GLDepthTexture2D& texture);
        void setTexture(const GLTexture2DArray& texture, uint8_t index);
        void setTexture(const GLDepthTexture2DArray& texture, uint8_t index);
    };
    
}

#endif /* GLFSQuadTextureRenderingProgram_hpp */
