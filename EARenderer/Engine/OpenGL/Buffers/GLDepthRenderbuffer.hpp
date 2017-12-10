//
//  GLDepthRenderbuffer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 10.12.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLDepthRenderbuffer_hpp
#define GLDepthRenderbuffer_hpp

#include "GLRenderbuffer.hpp"

namespace EARenderer {
    
    class GLDepthRenderbuffer: public GLRenderbuffer {
    public:
        GLDepthRenderbuffer(const Size2D& size);
    };
    
}

#endif /* GLDepthRenderbuffer_hpp */
