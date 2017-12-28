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
    public:
        using SamplerClosure = const std::function<void(const GLTexture2DSampler& sampler)>&;
        
        GLTexture2D(const Size2D& size);
        GLTexture2D(const std::string& fileName);
        
        const GLubyte* pixelBuffer() const;
        void sampleTexels(SamplerClosure samplerClosure) const;
    };
    
}

#endif /* GLTexture2D_hpp */
