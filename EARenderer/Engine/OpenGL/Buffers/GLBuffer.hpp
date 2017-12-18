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
    private:
        GLint mBindingPoint;
        
    public:        
        GLBuffer(GLint bindingPoint) {
            glGenBuffers(1, &mName);
            mBindingPoint = bindingPoint;
        }
        
        ~GLBuffer() override {
            glDeleteBuffers(1, &mName);
        }
        
        GLBuffer(GLBuffer&& that) = default;
        GLBuffer& operator=(GLBuffer&& rhs) = default;
        
        void bind() const override {
            glBindBuffer(mBindingPoint, mName);
        }
        
        void initialize(const std::vector<DataType>& data) {
            bind();
            glBufferData(mBindingPoint, data.size() * sizeof(DataType), data.data(), GL_STATIC_DRAW);
        }
        
        void initialize(const DataType *data, uint64_t size) {
            bind();
            glBufferData(mBindingPoint, size * sizeof(DataType), data, GL_STATIC_DRAW);
        };
    };
    
}

#endif /* GLBuffer_hpp */
