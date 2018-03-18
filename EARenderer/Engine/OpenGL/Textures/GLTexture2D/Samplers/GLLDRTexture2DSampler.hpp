//
//  GLLDRTexture2DSampler.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLLDRTexture2DSampler_hpp
#define GLLDRTexture2DSampler_hpp

#include "GLTexture2DSampler.hpp"

#include <memory>
#include <OpenGL/gltypes.h>

namespace EARenderer {

    class GLLDRTexture2D;

    class GLLDRTexture2DSampler: public GLTexture2DSampler {
    private:
        friend GLLDRTexture2D;

        std::unique_ptr<GLubyte> mPixelBuffer;

        GLLDRTexture2DSampler(const GLLDRTexture2D& texture, uint8_t mipLevel);

    public:
        Color sample(int32_t x, int32_t y) const override;
        Color sample(const glm::vec2& normalizedCoords) const override;
    };

}

#endif /* GLLDRTexture2DSampler_hpp */
