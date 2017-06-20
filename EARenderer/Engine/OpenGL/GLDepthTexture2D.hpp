//
//  GLDepthTexture2D.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLDepthTexture2D_hpp
#define GLDepthTexture2D_hpp

#include "GLNamedObject.hpp"
#include "GLBindable.hpp"
#include "Size.hpp"

namespace EARenderer {
    
    class GLDepthTexture2D: public GLNamedObject, public GLBindable {
    private:
        Size mSize;
        
    public:
        using GLNamedObject::GLNamedObject;
        GLDepthTexture2D(const Size& size);
        GLDepthTexture2D(GLDepthTexture2D&& that) = default;
        GLDepthTexture2D& operator=(GLDepthTexture2D&& rhs) = default;
        ~GLDepthTexture2D() override;
        
        const Size& size() const;
        
        void bind() const override;
    };
    
}

#endif /* GLDepthTexture2D_hpp */
