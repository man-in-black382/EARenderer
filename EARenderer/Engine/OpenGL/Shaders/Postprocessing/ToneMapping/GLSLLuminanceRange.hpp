//
//  GLSLLuminanceRange.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.08.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLLuminanceRange_hpp
#define GLSLLuminanceRange_hpp

#include "GLProgram.hpp"
#include "GLTexture2D.hpp"

namespace EARenderer {

    class GLSLLuminanceRange: public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLLuminanceRange();

        void setLuminance(const GLFloatTexture2D<GLTexture::Float::RG16F>& luminance);
        void setMipLevel(int8_t mipLevel);
    };

}

#endif /* GLSLLuminanceRange_hpp */
