//
//  GLSLGaussianBlur.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/13/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLGaussianBlur_hpp
#define GLSLGaussianBlur_hpp

#include "GLProgram.hpp"

#include <glm/mat4x4.hpp>

namespace EARenderer {

    class GLSLGaussianBlur: public GLProgram {
    public:
        using GLProgram::GLProgram;

        enum class BlurDirection { Vertical, Horizontal };

        GLSLGaussianBlur();

        void setBlurDirection(BlurDirection direction);
    };

}

#endif /* GLSLGaussianBlur_hpp */
