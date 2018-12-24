//
//  GLSLLuminanceHistogram.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 8/16/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLLuminanceHistogram_hpp
#define GLSLLuminanceHistogram_hpp

#include "GLProgram.hpp"
#include "GLTexture2D.hpp"

namespace EARenderer {

    class GLSLLuminanceHistogram : public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLLuminanceHistogram();

        void setLuminance(const GLFloatTexture2D<GLTexture::Float::RG16F> &luminance);

        void setHistogramWidth(size_t width);
    };

}

#endif /* GLSLLuminanceHistogram_hpp */
