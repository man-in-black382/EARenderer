//
//  GLSkyboxProgram.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 22.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLSkyboxProgram_hpp
#define GLSkyboxProgram_hpp

#include "GLProgram.hpp"
#include "GLTextureCubemap.hpp"

#include <glm/mat4x4.hpp>

namespace EARenderer {
    
    class GLSLSkybox: public GLProgram {
    public:        
        GLSLSkybox();
        
        void setViewMatrix(const glm::mat4& matrix);
        void setProjectionMatrix(const glm::mat4& matrix);
        void setEquirectangularMap(const GLFloatTexture2D<GLTexture::Float::RGB16F>& equireqMap);
        
        template<class TextureFormat, TextureFormat Format>
        void setCubemap(const GLTextureCubemap<TextureFormat, Format>& cubemap) {
            setUniformTexture(ctcrc32("uCubeMapTexture"), cubemap);
            glUniform1i(uniformByNameCRC32(ctcrc32("uIsCube")).location(), GL_TRUE);
        }
    };
    
}

#endif /* GLSkyboxProgram_hpp */
