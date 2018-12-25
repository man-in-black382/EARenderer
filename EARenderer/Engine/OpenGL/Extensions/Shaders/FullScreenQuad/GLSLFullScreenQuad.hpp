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

namespace EARenderer {

    class GLSLFullScreenQuad : public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLFullScreenQuad();

        template<class TextureFormat, TextureFormat Format>
        void setTexture(const GLTexture2D<TextureFormat, Format> &texture) {
            setUniformTexture(ctcrc32("uTexture"), texture);
            glUniform1i(uniformByNameCRC32(ctcrc32("uShouldSampleArray")).location(), GL_FALSE);
            glUniform1i(uniformByNameCRC32(ctcrc32("uShouldSample3DTexture")).location(), GL_FALSE);
        }

        template<class TextureFormat, TextureFormat Format>
        void setTexture(const GLTexture2DArray<TextureFormat, Format> &texture, size_t layer) {
            setUniformTexture(ctcrc32("uTextureArray"), texture);
            glUniform1i(uniformByNameCRC32(ctcrc32("uIndex")).location(), (GLint) layer);
            glUniform1i(uniformByNameCRC32(ctcrc32("uShouldSampleArray")).location(), GL_TRUE);
            glUniform1i(uniformByNameCRC32(ctcrc32("uShouldSample3DTexture")).location(), GL_FALSE);
        }

        void setTexture(const GLTexture3D &texture, float depth);

        void setApplyToneMapping(bool toneMap);
    };

}

#endif /* GLFSQuadTextureRenderingProgram_hpp */
