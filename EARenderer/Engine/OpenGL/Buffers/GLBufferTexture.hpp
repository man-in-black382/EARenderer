//
//  GLBufferTexture.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/18/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLBufferTexture_hpp
#define GLBufferTexture_hpp

#include "GLBuffer.hpp"

namespace EARenderer {
    
    template <typename DataType>
    class GLBufferTextureBase: public GLBuffer<DataType> {
    private:
        GLenum mInternalFormat;
        
    public:
        GLBufferTextureBase(GLenum internalFormat)
        :
        GLBuffer<DataType>(GL_TEXTURE_BUFFER),
        mInternalFormat(internalFormat)
        { }
        
        void bind() const override {
            glTexBuffer(GL_TEXTURE_BUFFER, mInternalFormat, GLBuffer<DataType>::mName);
        }
    };

}

#endif /* GLBufferTexture_hpp */
