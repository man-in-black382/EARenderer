//
//  GLBufferWritingSession.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/19/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLBufferWriter_hpp
#define GLBufferWriter_hpp

#include <stdlib.h>
#include "MemoryUtils.hpp"
#include "StringUtils.hpp"
#include "LogUtils.hpp"

namespace EARenderer {

    template<typename DataType>
    class GLBuffer;

    template<typename DataType>
    class GLBufferWritingSession {
    private:
        friend GLBuffer<DataType>;

        using DeallocationCallback = std::function<void()>;

        struct MemoryLocation {
            const DataType *data = nullptr;
            size_t count = 1;
            size_t nextOffset = 0;
        };

        const GLBuffer<DataType> *mBuffer;
        GLint mBindingPoint;
        std::vector<MemoryLocation> mDataQueue;

        GLBufferWritingSession(const GLBuffer<DataType> *buffer, GLint bindingPoint)
                : mBuffer(buffer), mBindingPoint(bindingPoint) {}

    public:
        size_t enqueueData(const DataType *data, size_t count = 1) {

            if (count == 0) {
                throw std::invalid_argument("Count must bo greater than 0");
            }

            size_t dataByteSize = sizeof(DataType) * count;
            size_t alignedOffset = mDataQueue.size() ? mDataQueue.back().nextOffset : 0;
            size_t padding = Utils::Memory::Padding(dataByteSize, mBuffer->alignment());
            size_t newOffset = alignedOffset + dataByteSize + padding;

            if ((alignedOffset / sizeof(DataType) + count) >= mBuffer->count()) {
                throw std::range_error("Attempt to queue data write outside of the buffer");
            }

            mDataQueue.push_back({data, count, newOffset});

            return alignedOffset;
        }

        void flush() {
            if (mDataQueue.empty()) {
                return;
            }

            mBuffer->bind();
            auto ptr = reinterpret_cast<DataType *>(glMapBufferRange(mBindingPoint, 0, mDataQueue.back().nextOffset, GL_MAP_WRITE_BIT));
            size_t offset = 0;
            for (auto &location : mDataQueue) {
                memcpy(ptr + offset, location.data, sizeof(DataType) * location.count);
                offset = location.nextOffset;
            }
            glUnmapBuffer(mBindingPoint);
        }

    };

}

#endif /* GLBufferWriter_hpp */
