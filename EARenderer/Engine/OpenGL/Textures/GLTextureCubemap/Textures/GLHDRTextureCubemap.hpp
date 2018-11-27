//
//  GLHDRTextureCubemap.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLHDRTextureCubemap_hpp
#define GLHDRTextureCubemap_hpp

#include "GLTextureCubemap.hpp"
#include "GLHDRCubemapSampler.hpp"

#include <array>
#include <vector>
#include <glm/vec3.hpp>

namespace EARenderer {
    
    class GLHDRTextureCubemap: public GLTextureCubemap {
    public:
        GLHDRTextureCubemap(const Size2D& size, Sampling::Filter filter = Sampling::Filter::Trilinear);
        GLHDRTextureCubemap(const std::array<std::vector<glm::vec3>, 6>& pixels, Sampling::Filter filter = Sampling::Filter::Trilinear);

        ~GLHDRTextureCubemap() = default;

        GLHDRCubemapSampler sampleTexels(int32_t mipLevel = 0) const;
    };
    
}

#endif /* GLHDRTextureCubemap_hpp */
