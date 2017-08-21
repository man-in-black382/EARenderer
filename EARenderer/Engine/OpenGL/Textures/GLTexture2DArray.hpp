//
//  GLTexture2DArray.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 02.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLTexture2DArray_hpp
#define GLTexture2DArray_hpp

#include "GLLayeredTexture.hpp"

namespace EARenderer {
    
    class GLTexture2DArray: public GLLayeredTexture {
    public:
        GLTexture2DArray(const Size2D& size, GLsizei layers);
    };
    
}

#endif /* GLTexture2DArray_hpp */
