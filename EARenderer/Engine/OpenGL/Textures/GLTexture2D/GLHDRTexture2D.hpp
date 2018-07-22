//
//  GLHDRTexture2D.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLHDRTexture2D_hpp
#define GLHDRTexture2D_hpp

#include "GLTexture2D.hpp"

#include <vector>

namespace EARenderer {
    
    class GLHDRTexture2D: public GLTexture2D<GLTexture::Float, GLTexture::Float::RGBA32F> {
    public:
        GLHDRTexture2D() = default;
        GLHDRTexture2D(const Size2D& size, GLDepthTexture2D::Filter filter = GLDepthTexture2D::Filter::Bilinear) {

        }
        GLHDRTexture2D(const std::string& imagePath) {

        }
        GLHDRTexture2D(const std::vector<float>& floats, const Size2D& size) {
            
        }
        ~GLHDRTexture2D() = default;
    };
    
}

#endif /* GLHDRTexture2D_hpp */
