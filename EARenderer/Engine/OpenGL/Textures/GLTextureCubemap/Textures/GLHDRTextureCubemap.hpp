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

namespace EARenderer {
    
    class GLHDRTextureCubemap: public GLTextureCubemap {
    public:
        GLHDRTextureCubemap(const Size2D& size, Filter filter = Filter::Trilinear);
        ~GLHDRTextureCubemap() = default;

        void sampleTexels(int32_t mipLevel, SamplerClosure samplerClosure) const override;
    };
    
}

#endif /* GLHDRTextureCubemap_hpp */
