//
//  GLTexture2DArray.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 02.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLDepthTexture2DArray_hpp
#define GLDepthTexture2DArray_hpp

#include "GLTexture2DArray.hpp"

namespace EARenderer {
    
    class GLDepthTexture2DArray: public GLTexture2DArray {
    public:        
        GLDepthTexture2DArray(const Size2D& size, GLsizei layers);
        ~GLDepthTexture2DArray() = default;
    };
    
}

#endif /* GLTexture2DArray_hpp */
