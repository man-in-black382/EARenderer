//
//  GLTexture2D.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 19.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLTexture2D_hpp
#define GLTexture2D_hpp

#include "GLTexture.hpp"

namespace EARenderer {
    
    class GLTexture2D: public GLTexture {
    public:        
        GLTexture2D(const Size2D& size);
        GLTexture2D(const std::string& fileName);
    };
    
}

#endif /* GLTexture2D_hpp */
