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
    
    class GLSkyboxProgram: public GLProgram {
    public:
        using GLProgram::GLProgram;
        
        void setViewMatrix(const glm::mat4& matrix);
        void setProjectionMatrix(const glm::mat4& matrix);
        void setCubemap(const GLTextureCubemap& cubemap);
    };
    
}

#endif /* GLSkyboxProgram_hpp */
