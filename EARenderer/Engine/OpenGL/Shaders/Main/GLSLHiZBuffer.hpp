//
//  GLSLHiZBuffer.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 7/16/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLHiZBuffer_hpp
#define GLSLHiZBuffer_hpp

#include "GLProgram.hpp"
#include "GLTexture2D.hpp"

namespace EARenderer {

    class GLSLHiZBuffer : public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLHiZBuffer();

        void setTexture(const GLFloatTexture2D<GLTexture::Float::R32F> &texture);

        void setMipLevel(int8_t mipLevel);
    };

}


#endif /* GLSLHiZBuffer_hpp */
