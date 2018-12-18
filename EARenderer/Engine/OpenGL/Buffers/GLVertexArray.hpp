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
#include <stdexcept>
#include <memory>

#include "GLNamedObject.hpp"
#include "GLVertexArrayBuffer.hpp"
#include "GLElementArrayBuffer.hpp"
#include "GLVertexAttribute.hpp"

namespace EARenderer {

    template<typename Vertex>
    class GLVertexArray : public GLNamedObject {
    private:
        GLVertexArrayBuffer<Vertex> mVertexBuffer;
        GLElementArrayBuffer mIndexBuffer;

    public:

#pragma mark - Lifecycle

        GLVertexArray() {
            glGenVertexArrays(1, &mName);
        }

        ~GLVertexArray() override {
            glDeleteVertexArrays(1, &mName);
        }

        GLVertexArray(GLVertexArray &&that) = default;

        GLVertexArray &operator=(GLVertexArray &&rhs) = default;

#pragma mark - Binbable

        void bind() const {
            glBindVertexArray(mName);
        }

#pragma mark - Lazy initialization

        void initialize(const Vertex *vertices, uint64_t verticesSize,
                const GLushort *indices, uint64_t indicesSize,
                const std::vector<GLVertexAttribute> &attributes) {
            bind();

            mVertexBuffer.initialize(vertices, verticesSize);
            if (indices) {mIndexBuffer.initialize(indices, indicesSize);}

            GLuint offset = 0;
            for (GLuint location = 0; location < attributes.size(); location++) {
                glEnableVertexAttribArray(location);
                const GLVertexAttribute &attribute = attributes[location];
                glVertexAttribPointer(location, attribute.components, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offset));
                glVertexAttribDivisor(location, attribute.divisor);
                offset += attribute.bytes;
            }
        }

        void initialize(const Vertex *vertices, uint64_t verticesSize, const std::vector<GLVertexAttribute> &attributes) {
            initialize(vertices, verticesSize, nullptr, 0, attributes);
        }

        void initialize(const std::vector<Vertex> &vertices, const std::vector<GLVertexAttribute> &attributes) {
            initialize(vertices.data(), vertices.size(), nullptr, 0, attributes);
        }

        void initialize(const std::vector<Vertex> &vertices, const std::vector<GLushort> &indices, const std::vector<GLVertexAttribute> &attributes) {
            initialize(vertices.data(), vertices.size(), nullptr, 0, attributes);
        }

        template<typename T>
        void useExternalBuffer(const GLVertexArrayBuffer<T> &buffer, const std::vector<GLVertexAttribute> &attributes) {
            bind();
            buffer.bind();

            GLuint offset = 0;
            for (auto &attribute : attributes) {
                if (attribute.location == GLVertexAttribute::LocationAutomatic) {
                    throw std::logic_error("You're trying to use external buffer with attribute's location set to Automatic which will probably override attributes taken from internal buffer and replace data at location 0 and so forth");
                }

                glEnableVertexAttribArray(attribute.location);
                glVertexAttribPointer(attribute.location, attribute.components, GL_FLOAT, GL_FALSE, sizeof(T), reinterpret_cast<void *>(offset));
                glVertexAttribDivisor(attribute.location, attribute.divisor);
                offset += attribute.bytes;
            }
        }

#pragma mark - Getters

        const GLVertexArrayBuffer<Vertex> &vertexBuffer() const {
            return mVertexBuffer;
        }

        const GLVertexArrayBuffer<Vertex> &indexBuffer() const {
            return mIndexBuffer;
        }
    };

}

#endif /* GLVertexArray_hpp */
