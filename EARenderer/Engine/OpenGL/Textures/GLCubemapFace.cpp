//
//  GLCubemapFace.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/5/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLCubemapFace.hpp"

namespace EARenderer {
    
    GLCubemapFace CubemapFaceFromIndex(uint8_t index) {
        switch (index) {
            default:
            case 0: return GLCubemapFace::PositiveX;
            case 1: return GLCubemapFace::NegativeX;
            case 2: return GLCubemapFace::PositiveY;
            case 3: return GLCubemapFace::NegativeY;
            case 4: return GLCubemapFace::PositiveZ;
            case 5: return GLCubemapFace::NegativeZ;
        }
    }
    
}
