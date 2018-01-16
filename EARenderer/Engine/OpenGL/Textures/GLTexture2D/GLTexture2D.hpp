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
#include "GLTexture2DSampler.hpp"

namespace EARenderer {
    
    class GLTexture2D: public GLTexture {
    private:
        friend GLTexture2DSampler;
        
        const GLubyte* pixelBuffer(int32_t mipLevel) const;
        
    public:
        using SamplerClosure = const std::function<void(const GLTexture2DSampler& sampler)>&;
        
        GLTexture2D(const Size2D& size);
        GLTexture2D(const std::string& fileName);
        
        void sampleTexels(SamplerClosure samplerClosure, int32_t mipLevel = 0) const;
    };
    
}

#endif /* GLTexture2D_hpp */
