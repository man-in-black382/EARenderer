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

    GLTextureSampler::GLTextureSampler(const Size2D textureSize, int32_t mipLevel)
    :
    mMipSize(textureSize.transformedBy(glm::vec2(std::pow(0.5, mipLevel))))
    { }

    GLTextureSampler::~GLTextureSampler() { }
    
}
