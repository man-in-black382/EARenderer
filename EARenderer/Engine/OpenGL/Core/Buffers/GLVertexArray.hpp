//
//  GLVertexArray.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLVertexArray_hpp
#define GLVertexArray_hpp

#include <OpenGL/OpenGL.h>
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
        std::unique_ptr<GLVertexArrayBuffer<Vertex>> mVertexBuffer;
        std::unique_ptr<GLElementArrayBuffer> mIndexBuffer;

        void hookUpBuffers(const GLVertexAttribute *attributes, size_t attributeCount) {
            bind();
            mVertexBuffer->bind();

            if (mIndexBuffer) {
                mIndexBuffer->bind();
            }

            GLuint offset = 0;
            for (GLuint location = 0; location < attributeCount; location++) {
                glEnableVertexAttribArray(location);
                const GLVertexAttribute &attribute = attributes[location];
                glVertexAttribPointer(location, attribute.components, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offset));
                glVertexAttribDivisor(location, attribute.divisor);
                offset += attribute.bytes;
            }
        }

    public:

#pragma mark - Lifecycle

        template<
                template<class...> class VertexContainer,
                template<class...> class AttributeContainer
        >
        static auto Create(const VertexContainer<Vertex> &vertices, const AttributeContainer<GLVertexAttribute> &attributes) {
            return GLVertexArray(vertices.data(), vertices.size(), attributes.data(), attributes.size());
        }

        template<
                template<class...> class VertexContainer,
                template<class...> class IndexContainer,
                template<class...> class AttributeContainer
        >
        static auto Create(const VertexContainer<Vertex> &vertices, const IndexContainer<GLushort> &indices, const AttributeContainer<GLVertexAttribute> &attributes) {
            return GLVertexArray(vertices.data(), vertices.size(), indices.data(), indices.size(), attributes.data(), attributes.size());
        }

        GLVertexArray(const Vertex *vertices, size_t vertexCount, const GLVertexAttribute *attributes, size_t attributeCount)
                : mVertexBuffer(std::make_unique<GLVertexArrayBuffer<Vertex>>(vertices, vertexCount)) {

            glGenVertexArrays(1, &mName);
            hookUpBuffers(attributes, attributeCount);
        }

        GLVertexArray(const Vertex *vertices, size_t vertexCount, const GLushort *indices, size_t indexCount, const GLVertexAttribute *attributes, size_t attributeCount)
                : mVertexBuffer(std::make_unique<GLVertexArrayBuffer<Vertex>>(vertices, vertexCount)),
                  mIndexBuffer(std::make_unique<GLElementArrayBuffer>(indices, indexCount)) {

            glGenVertexArrays(1, &mName);
            hookUpBuffers(attributes, attributeCount);
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

        template<typename T, template<class...> class AttributeContainer>
        void useExternalBuffer(const GLVertexArrayBuffer<T> &buffer, const AttributeContainer<GLVertexAttribute> &attributes) {
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

    };

}

#endif /* GLVertexArray_hpp */
