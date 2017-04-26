//
//  GLVertexArrayBuffer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLVertexArrayBuffer_hpp
#define GLVertexArrayBuffer_hpp

#include "GLBuffer.inl"
#include <OpenGL/gl.h>

namespace EARenderer {
    
    template <typename Vertex>
    class GLVertexArrayBuffer : public GLBuffer<Vertex> {
    public:
        using GLBuffer<Vertex>::GLBuffer;
//        GLVertexArrayBuffer(const GLVertexArrayBuffer& that) = delete;
        GLVertexArrayBuffer(GLVertexArrayBuffer&& that) = default;
//        GLVertexArrayBuffer& operator=(const GLVertexArrayBuffer& rhs) = delete;
        GLVertexArrayBuffer& operator=(GLVertexArrayBuffer&& rhs) = default;
        
        void bind() const override {
            glBindBuffer(GL_ARRAY_BUFFER, GLBuffer<Vertex>::mName);
        }
        
        void initialize(const std::vector<Vertex>& data) override {
            bind();
            glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Vertex), data.data(), GL_STATIC_DRAW);
        }
    };
    
}

#endif /* GLVertexArrayBuffer_hpp */
