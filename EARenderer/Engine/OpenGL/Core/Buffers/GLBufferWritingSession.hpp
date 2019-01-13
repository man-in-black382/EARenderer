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

namespace EARenderer {

    template<typename DataType>
    class GLBuffer;

    template<typename DataType>
    class GLBufferWritingSession {
    private:
        friend GLBuffer<DataType>;

        using DeallocationCallback = std::function<void()>;

        std::byte *mMappedBuffer = nullptr;
        size_t mObjectCapacity = 0;
        size_t mAlignment = 1;
        DeallocationCallback mDeallocationCallback;

        GLBufferWritingSession(std::byte *mappedAddress, size_t objectCapacity, size_t alignment, const DeallocationCallback &deallocationCallback)
                : mMappedBuffer(mappedAddress), mObjectCapacity(objectCapacity), mAlignment(alignment), mDeallocationCallback(deallocationCallback) {}

    public:
        using AlignedOffset = size_t;

        ~GLBufferWritingSession() {
            mDeallocationCallback();
        }

        /// Copies client's memory region into a mapped GL buffer.
        /// Inserts padding at the end of the copied data according to the alignment parameter provided at the buffer construction stage.
        /// @param data Pointer to the client's data.
        /// @param count Amount of data objects to be copied.
        /// @param alignedOffset Byte offset at which data will be placed inside a GL buffer.
        /// Must be aligned to the alignment parameter provided at the buffer construction stage.
        /// @return Aligned byte offset at which the next data chunk should be placed.
        AlignedOffset write(const DataType* data, size_t count = 1, AlignedOffset alignedOffset = 0) {

            if (count == 0) {
                throw std::invalid_argument("Count must bo greater than 0. You cannot copy 0 objects.");
            }

            if ((alignedOffset % mAlignment) != 0) {
                throw std::invalid_argument(string_format("Buffer offset %d is not aligned to the alignment of %d", alignedOffset, mAlignment));
            }

            if ((alignedOffset / sizeof(DataType) + count) >= mObjectCapacity) {
                throw std::range_error("Attempt to write data outside of the buffer");
            }

            size_t dataByteSize = sizeof(DataType) * count;
            auto padding = Utils::Memory::Padding(dataByteSize, mAlignment);
            memcpy(mMappedBuffer + alignedOffset, data, dataByteSize);

            size_t newOffset = alignedOffset + dataByteSize + padding;
            return newOffset;
        }
    };

}

#endif /* GLBufferWriter_hpp */
