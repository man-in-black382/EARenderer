//
//  GLTextureSampler.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLTextureSampler.hpp"

#include <cmath>

namespace EARenderer {

#pragma mark - Lifecycle

    GLTextureSampler::GLTextureSampler(const GLTexture& texture, int32_t mipLevel) {
        texture.bind();
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        GLint w = 0;
        GLint h = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_WIDTH, &w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_HEIGHT, &h);
        mMipSize.width = w;
        mMipSize.height = h;
    }

    GLTextureSampler::~GLTextureSampler() { }
    
}
