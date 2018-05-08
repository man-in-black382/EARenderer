//
//  GLTextureSampler.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLTextureSampler_hpp
#define GLTextureSampler_hpp

#include "GLTexture.hpp"

#include <stdlib.h>

namespace EARenderer {

    class GLTextureSampler {
    protected:
        Size2D mMipSize;

        GLTextureSampler(const GLTexture& texture, int32_t mipLevel);

    public:
        virtual ~GLTextureSampler() = 0;
    };

}

#endif /* GLTextureSampler_hpp */
