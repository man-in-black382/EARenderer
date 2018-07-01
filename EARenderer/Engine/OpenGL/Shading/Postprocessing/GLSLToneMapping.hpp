//
//  GLSLToneMapping.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 28.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLToneMapping_hpp
#define GLSLToneMapping_hpp

#include "GLProgram.hpp"
#include "GLHDRTexture2D.hpp"

#include <glm/mat4x4.hpp>
#include <vector>

namespace EARenderer {

    class GLSLToneMapping: public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLToneMapping();

        void setImage(const GLHDRTexture2D& image);
    };

}

#endif /* GLSLToneMapping_hpp */
