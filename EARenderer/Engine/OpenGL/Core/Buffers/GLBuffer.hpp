//
//  GLBuffer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLBuffer_hpp
#define GLBuffer_hpp

#include <vector>

#include "GLNamedObject.hpp"
#include "GLBufferWriter.hpp"

namespace EARenderer {

    template<typename DataType>
    class GLBuffer : public GLNamedObject {
    private:
        GLint mBindingPoint;
        GLenum mUsageMode;
        size_t mSize;

    public:

#pragma mark - Types

        using WriteContextClosure = std::function<void(GLBufferWriter<DataType> writer)>;

#pragma mark - Lifecycle

        GLBuffer(GLint bindingPoint, GLenum usageMode)
                : mBindingPoint(bindingPoint), mUsageMode(usageMode) {
            glGenBuffers(1, &mName);
        }

        ~GLBuffer() override {
            glDeleteBuffers(1, &mName);
        }

        GLBuffer(GLBuffer &&that) = default;

        GLBuffer &operator=(GLBuffer &&rhs) = default;

#pragma mark - Getters

        /// Size in bytes
        /// @return size of the buffer
        size_t size() const {
            return mSize;
        }

#pragma mark - Binding

        virtual void bind() const {
            glBindBuffer(mBindingPoint, mName);
        }

#pragma mark - Helpers

        void initialize(const std::vector<DataType> &data) {
            bind();
            size_t bytesCount = data.size() * sizeof(DataType);
            glBufferData(mBindingPoint, bytesCount, data.data(), mUsageMode);
            mSize = bytesCount;
        }

        void initialize(const DataType *data, uint64_t size) {
            bind();
            size_t bytesCount = size * sizeof(DataType);
            glBufferData(mBindingPoint, bytesCount, data, mUsageMode);
            mSize = bytesCount;
        }

        void write(const WriteContextClosure &contextClosure) {
            bind();
            DataType *data = reinterpret_cast<DataType *>(glMapBuffer(mBindingPoint, GL_WRITE_ONLY));
            contextClosure(GLBufferWriter<DataType>(data, mSize));
            glUnmapBuffer(mBindingPoint);
        }
    };

}

#endif /* GLBuffer_hpp */
