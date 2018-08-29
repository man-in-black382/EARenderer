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
#include "GLTexture2D.hpp"

#include <glm/mat4x4.hpp>
#include <vector>

namespace EARenderer {

    class GLSLToneMapping: public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLToneMapping();

        void setImage(const GLFloatTexture2D<GLTexture::Float::RGBA16F>& image);
        void setExposure(const GLFloatTexture2D<GLTexture::Float::R16F>& exposure);
    };

}

#endif /* GLSLToneMapping_hpp */
