
//
//  GLSLBloom.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 27.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLBloom.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLBloom::GLSLBloom()
    :
    GLProgram("FullScreenQuad.vert", "Bloom.frag", "")
    { }

#pragma mark - Lifecycle

    void GLSLBloom::setTextures(const GLFloatTexture2D& baseImageTexture,
                                const GLFloatTexture2D& smallBlurTexture,
                                const GLFloatTexture2D& mediumBlurTexture,
                                const GLFloatTexture2D& largeBlurTexture)
    {
        setUniformTexture(uint32_constant<ctcrc32("uBaseImageTexture")>, baseImageTexture);
        setUniformTexture(uint32_constant<ctcrc32("uSmallBlurTexture")>, smallBlurTexture);
        setUniformTexture(uint32_constant<ctcrc32("uMediumBlurTexture")>, mediumBlurTexture);
        setUniformTexture(uint32_constant<ctcrc32("uLargeBlurTexture")>, largeBlurTexture);
    }

    void GLSLBloom::setTextureWeights(float smallBlurWeight, float mediumBlurWeight, float largeBlurWeight) {
        glUniform1f(uniformByNameCRC32(uint32_constant<ctcrc32("uSmallBlurWeight")>).location(), smallBlurWeight);
        glUniform1f(uniformByNameCRC32(uint32_constant<ctcrc32("uMediumBlurWeight")>).location(), mediumBlurWeight);
        glUniform1f(uniformByNameCRC32(uint32_constant<ctcrc32("uLargeBlurWeight")>).location(), largeBlurWeight);
    }

}
