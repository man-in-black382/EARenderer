//
//  GLTexture2DArray.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 02.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLTexture2DArray_hpp
#define GLTexture2DArray_hpp

#include "GLNamedObject.hpp"
#include "GLBindable.hpp"
#include "Size2D.hpp"

namespace EARenderer {
    
    class GLDepthTexture2DArray: public GLNamedObject, public GLBindable {
    private:
        Size2D mSize;
        uint16_t mCapacity;
        
        void applyParameters();
        
    public:
        using GLNamedObject::GLNamedObject;
        
        GLDepthTexture2DArray(const Size2D& size, uint16_t capacity);
        
        GLDepthTexture2DArray(GLDepthTexture2DArray&& that) = default;
        GLDepthTexture2DArray& operator=(GLDepthTexture2DArray&& rhs) = default;
        ~GLDepthTexture2DArray() override;
        
        void bind() const override;
        
        const Size2D& size() const;
        uint16_t capacity() const;
    };
    
}

#endif /* GLTexture2DArray_hpp */
