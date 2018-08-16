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

#include <glm/mat4x4.hpp>
#include <vector>

namespace EARenderer {

    class GLSLLuminanceHistogram: public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLLuminanceHistogram();

        void setImage(const GLFloatTexture2D<GLTexture::Float::RGBA16F>& image);
        void setHistogramWidth(size_t width);
    };

}

#endif /* GLSLLuminanceHistogram_hpp */
