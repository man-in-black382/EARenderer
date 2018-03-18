//
//  GLTexture2DSampler.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/28/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLTexture2DSampler.hpp"
#include "GLTexture2D.hpp"

namespace EARenderer {

#pragma mark - Heplers

    size_t GLTexture2DSampler::bufferOffset(int32_t x, int32_t y) const {
        return (y * mMipSize.width + x) * 4;
    }

    size_t GLTexture2DSampler::bufferOffset(const glm::vec2& normalizedCoords) const {
        int32_t x = normalizedCoords.x * (mMipSize.width - 1);
        int32_t y = normalizedCoords.y * (mMipSize.height - 1);
        return (y * mMipSize.width + x) * 4;
    }

}
