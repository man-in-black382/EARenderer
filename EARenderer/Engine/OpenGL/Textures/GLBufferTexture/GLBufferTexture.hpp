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
    class GLBufferTexture: public GLNamedObject, public GLBindable {
    private:
        GLTextureBuffer<BufferDataType> mBuffer;
        
    public:
        GLBufferTexture(GLenum internalFormat)
        :
        mBuffer(internalFormat)
        {
            glGenTextures(1, &mName);
        }
        
        virtual ~GLBufferTexture() = 0;
        
        void bind() const override {
            glBindTexture(GL_TEXTURE_BUFFER, mName);
            mBuffer.bind();
        }
        
        GLTextureBuffer<BufferDataType>& buffer() {
            return mBuffer;
        }

        const GLTextureBuffer<BufferDataType>& buffer() const {
            return mBuffer;
        }
    };
        
    template <typename BufferDataType>
    GLBufferTexture<BufferDataType>::~GLBufferTexture() {
        glDeleteTextures(1, &mName);
    }
    
    // Specializing by data type
    
    template <typename BufferDataType>
    class GLIntegerBufferTexture: public GLBufferTexture<BufferDataType> {
    public:
        GLIntegerBufferTexture() : GLBufferTexture<BufferDataType>(GL_R32I) { }
    };

    template <typename BufferDataType>
    class GLInteger2BufferTexture: public GLBufferTexture<BufferDataType> {
    public:
        GLInteger2BufferTexture() : GLBufferTexture<BufferDataType>(GL_RG32I) { }
    };

    template <typename BufferDataType>
    class GLInteger3BufferTexture: public GLBufferTexture<BufferDataType> {
    public:
        GLInteger3BufferTexture() : GLBufferTexture<BufferDataType>(GL_RGB32I) { }
    };

    template <typename BufferDataType>
    class GLUIntegerBufferTexture: public GLBufferTexture<BufferDataType> {
    public:
        GLUIntegerBufferTexture() : GLBufferTexture<BufferDataType>(GL_R32UI) { }
    };

    template <typename BufferDataType>
    class GLUInteger2BufferTexture: public GLBufferTexture<BufferDataType> {
    public:
        GLUInteger2BufferTexture() : GLBufferTexture<BufferDataType>(GL_RG32UI) { }
    };
    template <typename BufferDataType>
    class GLUInteger3BufferTexture: public GLBufferTexture<BufferDataType> {
    public:
        GLUInteger3BufferTexture() : GLBufferTexture<BufferDataType>(GL_RGB32UI) { }
    };
    
    template <typename BufferDataType>
    class GLFloatBufferTexture: public GLBufferTexture<BufferDataType> {
    public:
        GLFloatBufferTexture() : GLBufferTexture<BufferDataType>(GL_R32F) { }
    };
    
    template <typename BufferDataType>
    class GLFloat3BufferTexture: public GLBufferTexture<BufferDataType> {
        public:
        GLFloat3BufferTexture() : GLBufferTexture<BufferDataType>(GL_RGB32F) { }
    };
    
    template <typename BufferDataType>
    class GLFloat4BufferTexture: public GLBufferTexture<BufferDataType> {
        public:
        GLFloat4BufferTexture() : GLBufferTexture<BufferDataType>(GL_RGBA32F) { }
    };
    
    
}

#endif /* GLBufferTexture_hpp */
