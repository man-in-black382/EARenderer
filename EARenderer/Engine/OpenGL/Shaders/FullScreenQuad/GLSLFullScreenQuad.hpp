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
#include "GLTexture2DArray.hpp"
#include "GLTexture3D.hpp"
#include "GLDepthTexture2DArray.hpp"

namespace EARenderer {
    
    class GLSLFullScreenQuad: public GLProgram {
    public:
        using GLProgram::GLProgram;
        
        GLSLFullScreenQuad();

        template<class TextureFormat, TextureFormat Format>
        void setTexture(const GLTexture2D<TextureFormat, Format>& texture) {
            setUniformTexture(uint32_constant<ctcrc32("uTexture")>, texture);
            glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uShouldSampleArray")>).location(), GL_FALSE);
            glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uShouldSample3DTexture")>).location(), GL_FALSE);
        }
        
        void setTexture(const GLTexture3D& texture, float depth);
        void setApplyToneMapping(bool toneMap);
    };
    
}

#endif /* GLFSQuadTextureRenderingProgram_hpp */
