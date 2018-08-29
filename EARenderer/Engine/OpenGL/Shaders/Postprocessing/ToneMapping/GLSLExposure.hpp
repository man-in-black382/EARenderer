//
//  GLSLExposure.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.08.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLExposure_hpp
#define GLSLExposure_hpp

#include "GLProgram.hpp"
#include "GLTexture2D.hpp"

#include <glm/mat4x4.hpp>
#include <vector>

namespace EARenderer {

    class GLSLExposure: public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLExposure();

        void setLuminanceHistogram(const GLFloatTexture2D<GLTexture::Float::R32F>& histogram);
    };

}

#endif /* GLSLExposure_hpp */
