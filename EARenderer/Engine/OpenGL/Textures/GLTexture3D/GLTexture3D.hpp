//
//  GLTexture3D.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 3/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLTexture3D_hpp
#define GLTexture3D_hpp

#include "GLTexture.hpp"

#include <vector>

namespace EARenderer {

    class GLTexture3D: public GLTexture {
    protected:
        size_t mDepth = 0;

        void initialize(const Size2D& size, size_t depth, Sampling::Filter filter, Sampling::WrapMode wrapMode, GLint internalFormat);
        void initialize(const Size2D& size, Sampling::Filter filter, Sampling::WrapMode wrapMode, GLint internalFormat, GLenum format, GLenum type,
                        const std::vector<void *> pixelData);
    
    public:
        GLTexture3D();
        virtual ~GLTexture3D() = 0;

        size_t depth() const;
    };

}

#endif /* GLTexture3D_hpp */
