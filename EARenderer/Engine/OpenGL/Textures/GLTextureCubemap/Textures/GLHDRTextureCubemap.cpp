//
//  GLHDRTextureCubemap.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLHDRTextureCubemap.hpp"
#include "GLHDRCubemapSampler.hpp"
#include "GLTextureUnitManager.hpp"
#include "stb_image.h"

namespace EARenderer {
    
#pragma mark - Lifecycle

    GLHDRTextureCubemap::GLHDRTextureCubemap(const Size2D& size, Sampling::Filter filter) {
        std::array<const void *, 6> nullptrs;
        nullptrs.fill(nullptr);
        initialize(size, filter, Sampling::WrapMode::ClampToEdge, GL_RGB32F, GL_RGB, GL_FLOAT, nullptrs);
    }

    GLHDRTextureCubemap::GLHDRTextureCubemap(const std::array<std::vector<glm::vec3>, 6>& pixels, Sampling::Filter filter) {
        Size2D estimation;
        std::array<const void *, 6> pixelData;

        for (size_t i = 0; i < 6; i++) {
            Size2D size = EstimatedSize(pixels[i].size());
            if (size.width > estimation.width && size.height > estimation.height) {
                estimation = size;
            }
            pixelData[i] = pixels[i].data();
        }

        initialize(estimation, filter, Sampling::WrapMode::ClampToEdge, GL_RGB32F, GL_RGB, GL_FLOAT, pixelData);
    }
    
#pragma mark - Public interface

    GLHDRCubemapSampler GLHDRTextureCubemap::sampleTexels(int32_t mipLevel) const {
//        bind();
        return GLHDRCubemapSampler(*this, mipLevel);
    }

}


