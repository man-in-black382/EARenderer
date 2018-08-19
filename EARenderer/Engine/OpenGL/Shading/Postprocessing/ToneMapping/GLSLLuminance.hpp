//
//  GLSLLuminance.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.08.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLLuminance_hpp
#define GLSLLuminance_hpp

#include "GLProgram.hpp"
#include "GLTexture2D.hpp"

namespace EARenderer {

    class GLSLLuminance: public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLLuminance();

        void setImage(const GLFloatTexture2D<GLTexture::Float::RGBA16F>& image);
    };

}

#endif /* GLSLLuminance_hpp */
