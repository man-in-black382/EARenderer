//
//  GLTexture2DArray.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 02.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLTexture2DArray_hpp
#define GLTexture2DArray_hpp

#include "GLTexture.hpp"

#include <vector>

namespace EARenderer {
    
    class GLTexture2DArray: public GLTexture {
    protected:
        size_t mLayers = 0;

        void initialize(const Size2D& size, size_t layers, Filter filter, WrapMode wrapMode, GLint internalFormat);
        void initialize(const Size2D& size, Filter filter, WrapMode wrapMode, GLint internalFormat, GLenum format, GLenum type, const std::vector<const void *>& pixelData);

    public:
        GLTexture2DArray();
        virtual ~GLTexture2DArray() = 0;

        size_t layers() const;
    };
    
}

#endif /* GLTexture2DArray_hpp */
