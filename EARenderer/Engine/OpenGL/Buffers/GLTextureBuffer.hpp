//
//  GLBufferTexture.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/18/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLTextureBuffer_hpp
#define GLTextureBuffer_hpp

#include "GLBuffer.hpp"

namespace EARenderer {
    
    template <typename DataType>
    class GLTextureBuffer: public GLBuffer<DataType> {
    private:
        GLenum mInternalFormat;
        
    public:
        GLTextureBuffer(GLenum internalFormat)
        :
        GLBuffer<DataType>(GL_TEXTURE_BUFFER, GL_STATIC_DRAW),
        mInternalFormat(internalFormat)
        { }
        
        void bind() const {
            GLBuffer<DataType>::bind();
            glTexBuffer(GL_TEXTURE_BUFFER, mInternalFormat, GLBuffer<DataType>::mName);
        }
    };

}

#endif /* GLTextureBuffer_hpp */
