//
//  GLVertexArray.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLVertexArray_hpp
#define GLVertexArray_hpp

#include <OpenGL/gl3.h>
#include <vector>

#include "GLNamedObject.hpp"
#include "GLBindable.hpp"
#include "GLVertexArrayBuffer.hpp"
#include "GLElementArrayBuffer.hpp"
#include "GLVertexAttribute.hpp"

namespace EARenderer {
    
    template <typename Vertex>
    class GLVertexArray: public GLNamedObject, public GLBindable {
    private:
        GLVertexArrayBuffer<Vertex> mVertexBuffer;
        GLElementArrayBuffer mIndexBuffer;
        
    public:
        GLVertexArray() {
            glGenVertexArrays(1, &mName);
        }
        
        ~GLVertexArray() override {
            glDeleteVertexArrays(1, &mName);
        }
        
        GLVertexArray(GLVertexArray&& that) = default;
        GLVertexArray& operator=(GLVertexArray&& rhs) = default;
        
        void bind() const override {
            glBindVertexArray(mName);
        }

        void initialize(const Vertex *vertices, uint64_t verticesSize,
                        const GLushort *indices, uint64_t indicesSize,
                        const std::vector<GLVertexAttribute>& attributes)
        {
            bind();

            mVertexBuffer.initialize(vertices, verticesSize);
            if (indices) { mIndexBuffer.initialize(indices, indicesSize); }

            GLuint offset = 0;
            for (GLuint location = 0; location < attributes.size(); location++) {
                glEnableVertexAttribArray(location);
                const GLVertexAttribute& attribute = attributes[location];
                glVertexAttribPointer(location, attribute.components, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offset));
                glVertexAttribDivisor(location, attribute.divisor);
                offset += attribute.bytes;
            }
        }

        void initialize(const Vertex *vertices, uint64_t verticesSize, const std::vector<GLVertexAttribute>& attributes) {
            initialize(vertices, verticesSize, nullptr, 0, attributes);
        }
        
        void initialize(const std::vector<Vertex>& vertices, const std::vector<GLVertexAttribute>& attributes) {
            initialize(vertices.data(), vertices.size(), nullptr, 0, attributes);
        }
        
        void initialize(const std::vector<Vertex>& vertices, const std::vector<GLushort>& indices, const std::vector<GLVertexAttribute>& attributes) {
            initialize(vertices.data(), vertices.size(), nullptr, 0, attributes);
        }
    };
    
}

#endif /* GLVertexArray_hpp */
