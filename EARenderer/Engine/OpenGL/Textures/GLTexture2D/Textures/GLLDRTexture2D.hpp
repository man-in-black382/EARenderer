//
//  GLLDRTexture2D.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLLDRTexture2D_hpp
#define GLLDRTexture2D_hpp

#include "GLTexture2D.hpp"
#include "GLLDRTexture2DSampler.hpp"

#include <vector>

namespace EARenderer {

    class GLLDRTexture2D: public GLTexture2D {
    public:
        GLLDRTexture2D(const Size2D& size, Filter filter = Filter::Trilinear);
        GLLDRTexture2D(const std::string& imagePath);
        GLLDRTexture2D(const std::vector<uint8_t> bytes);
        ~GLLDRTexture2D() = default;

        GLLDRTexture2DSampler sampleTexels(int32_t mipLevel = 0) const;
    };

}

#endif /* GLLDRTexture2D_hpp */
