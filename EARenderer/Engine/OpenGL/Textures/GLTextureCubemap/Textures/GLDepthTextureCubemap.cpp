    //
//  GLDepthTextureCubemap.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLDepthTextureCubemap.hpp"
#include "Macros.h"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLDepthTextureCubemap::GLDepthTextureCubemap(const Size2D& size) {
        std::array<const void *, 6> nullptrs;
        nullptrs.fill(nullptr);
        initialize(size, Filter::Bilinear, WrapMode::ClampToBorder, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, nullptrs);
    }
    
}
