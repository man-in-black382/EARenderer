//
//  GLBufferTexture.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/18/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLBufferTexture_hpp
#define GLBufferTexture_hpp

#include "GLNamedObject.hpp"
#include "GLBindable.hpp"
#include "GLTextureBuffer.hpp"

namespace EARenderer {
    
    template <typename BufferDataType>
    class GLBufferTextureAbstract: public GLNamedObject, public GLBindable {
    private:
        GLTextureBuffer<BufferDataType> mBuffer;
        
    public:
        GLBufferTextureAbstract(GLenum internalFormat)
        :
        mBuffer(internalFormat)
        {
            glGenTextures(1, &mName);
        }
        
        virtual ~GLBufferTextureAbstract() = 0;
        
        void bind() const override {
            glBindTexture(GL_TEXTURE_BUFFER, mName);
            mBuffer.bind();
        }
    };
        
    template <typename BufferDataType>
    GLBufferTextureAbstract<BufferDataType>::~GLBufferTextureAbstract() {
        glDeleteTextures(1, &mName);
    }
    
    // Specializing by data type
    
    class GLIntegerBufferTexture: public GLBufferTextureAbstract<int32_t> {
    public:
        GLIntegerBufferTexture() : GLBufferTextureAbstract<int32_t>(GL_R32I) { }
    };
    
    class GLFloatBufferTexture: public GLBufferTextureAbstract<float> {
    public:
        GLFloatBufferTexture() : GLBufferTextureAbstract<float>(GL_R32F) { }
    };
    
}

#endif /* GLBufferTexture_hpp */
