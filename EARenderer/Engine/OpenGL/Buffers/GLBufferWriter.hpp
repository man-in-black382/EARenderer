//
//  GLBufferWriter.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/19/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLBufferWriter_hpp
#define GLBufferWriter_hpp

#include <stdlib.h>

 

namespace EARenderer {
    
    template<typename DataType>
    class GLBuffer;
    
    template <typename DataType>
    class GLBufferWriter {
    private:
        friend GLBuffer<DataType>;
        
        DataType *mData = nullptr;
        uint64_t mBufferSize = 0;
        
        GLBufferWriter(DataType *data, uint64_t size)
        :
        mData(data),
        mBufferSize(size)
        { }
        
    public:
        void writeAt(uint64_t index, const DataType& data) {
            ASSERT(mBufferSize > 0, "Attempt to write to a zero-sized buffer. Did you forget to allocate space for data first?");
            ASSERT(index < mBufferSize, "Index out of range. Attempt to write to an OpenGL buffer outside of its boundaries.");
            mData[index] = data;
        }
    };
    
}

#endif /* GLBufferWriter_hpp */
