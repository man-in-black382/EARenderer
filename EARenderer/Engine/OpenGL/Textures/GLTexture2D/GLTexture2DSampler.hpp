//
//  GLTexture2DSampler.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/28/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLTexture2DSampler_hpp
#define GLTexture2DSampler_hpp

#include "Color.hpp"

#include <OpenGL/gl3.h>

namespace EARenderer {
    
    class GLTexture2D;
    
    class GLTexture2DSampler {
    private:
        friend GLTexture2D;
        
        const GLTexture2D *mTexture;
        const GLubyte *mPixelBuffer = nullptr;
        
        GLTexture2DSampler(const GLTexture2D* texture, int32_t mipLevel);
        
    public:
        ~GLTexture2DSampler();
        
        Color sample(int32_t x, int32_t y) const;
    };
    
}


#endif /* GLTexture2DSampler_hpp */
