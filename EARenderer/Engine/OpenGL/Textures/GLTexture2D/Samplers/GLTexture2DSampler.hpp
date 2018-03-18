//
//  GLTexture2DSampler.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/28/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLTexture2DSampler_hpp
#define GLTexture2DSampler_hpp

#include "GLTextureSampler.hpp"
#include "Color.hpp"

namespace EARenderer {

    class GLTexture2DSampler: public GLTextureSampler {
    protected:
        using GLTextureSampler::GLTextureSampler;

        size_t bufferOffset(int32_t x, int32_t y) const;
        size_t bufferOffset(const glm::vec2& normalizedCoords) const;
        
    public:
        virtual Color sample(int32_t x, int32_t y) const = 0;
        virtual Color sample(const glm::vec2& normalizedCoords) const = 0;
    };
    
}

#endif /* GLTexture2DSampler_hpp */
