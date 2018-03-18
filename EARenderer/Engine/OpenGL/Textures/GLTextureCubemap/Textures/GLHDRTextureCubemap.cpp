//
//  GLHDRTextureCubemap.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLHDRTextureCubemap.hpp"
#include "GLHDRCubemapSampler.hpp"
#include "stb_image.h"

namespace EARenderer {
    
#pragma mark - Lifecycle

    GLHDRTextureCubemap::GLHDRTextureCubemap(const Size2D& size, Filter filter) {
        std::array<void *, 6> nullptrs;
        nullptrs.fill(nullptr);
        initialize(size, filter, WrapMode::ClampToEdge, GL_RGB16F, GL_RGB, GL_FLOAT, nullptrs);
    }
    
#pragma mark - Public interface

    void GLHDRTextureCubemap::sampleTexels(int32_t mipLevel, SamplerClosure samplerClosure) const {
        bind();
        samplerClosure(GLHDRCubemapSampler(*this, mipLevel));
    }

}


