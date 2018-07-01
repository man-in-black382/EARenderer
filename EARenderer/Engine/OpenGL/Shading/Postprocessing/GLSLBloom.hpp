//
//  GLSLBloom.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 27.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLBloom_hpp
#define GLSLBloom_hpp

#include "GLProgram.hpp"
#include "GLHDRTexture2D.hpp"

#include <glm/mat4x4.hpp>
#include <vector>

namespace EARenderer {

    class GLSLBloom: public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLBloom();

        void setTextures(const GLHDRTexture2D& baseImageTexture,
                         const GLHDRTexture2D& smallBlurTexture,
                         const GLHDRTexture2D& mediumBlurTexture,
                         const GLHDRTexture2D& largeBlurTexture);

        void setTextureWeights(float smallBlurWeight, float mediumBlurWeight, float largeBlurWeight);
    };

}

#endif /* GLSLBloom_hpp */
