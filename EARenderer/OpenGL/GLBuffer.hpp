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
            GLuint name = 0;
            glGenBuffers(1, &name);
            mName = name;
        }
        
        ~GLBuffer() {
            glDeleteBuffers(1, &mName);
        }
        
        GLBuffer(const GLBuffer& that) = delete;
        GLBuffer(GLBuffer&& that) = delete;
        GLBuffer& operator=(const GLBuffer& rhs) = delete;
        GLBuffer& operator=(GLBuffer&& rhs) = delete;
        
        virtual void initialize(const std::vector<DataType>& data) = 0;
    };
    
}

#endif /* GLBuffer_hpp */
