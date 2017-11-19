//
//  GLCubemapFace.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLCubemapFace_hpp
#define GLCubemapFace_hpp

#include <OpenGL/gl3.h>

namespace EARenderer {
    
    enum class GLCubemapFace: GLenum {
        PositiveX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        NegativeX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        PositiveY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        NegativeY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        PositiveZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        NegativeZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };
    
}

#endif /* GLCubemapFace_hpp */
