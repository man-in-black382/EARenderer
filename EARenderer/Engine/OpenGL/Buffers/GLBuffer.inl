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
#include "GLBindable.hpp"

namespace EARenderer {
    
    template <typename DataType>
    class GLBuffer: public GLNamedObject, public GLBindable {
    public:        
        GLBuffer() {
            GLuint name = -1;
            glGenBuffers(1, &name);
            mName = name;
        }
        
        ~GLBuffer() override {
            glDeleteBuffers(1, &mName);
        }
        
        GLBuffer(GLBuffer&& that) = default;
        GLBuffer& operator=(GLBuffer&& rhs) = default;
        
        virtual void initialize(const std::vector<DataType>& data) = 0;
        virtual void initialize(const DataType *data, uint64_t size) = 0;
    };
    
}

#endif /* GLBuffer_hpp */
