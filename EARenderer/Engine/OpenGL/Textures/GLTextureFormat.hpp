//
//  GLTextureFormat.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 7/2/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLTextureFormat_hpp
#define GLTextureFormat_hpp

#include <OpenGL/gltypes.h>

namespace EARenderer {

    struct GLTextureFormat {
        GLint internalFormat;
        GLenum inputPixelFormat;
        GLenum inputPixelType;
    };

}

#endif /* GLTextureFormat_hpp */
