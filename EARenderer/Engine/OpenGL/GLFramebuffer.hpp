//
//  GLFramebuffer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLFramebuffer_hpp
#define GLFramebuffer_hpp

#include "GLNamedObject.hpp"
#include "GLBindable.hpp"
#include "GLDepthTexture2D.hpp"
#include "Geometry.hpp"

namespace EARenderer {
    
    class GLFramebuffer: public GLNamedObject, public GLBindable {
    private:
        Size mSize;
        
    public:
        using GLNamedObject::GLNamedObject;
        GLFramebuffer(const Size& size);
        GLFramebuffer(GLFramebuffer&& that) = default;
        GLFramebuffer& operator=(GLFramebuffer&& rhs) = default;
        ~GLFramebuffer() override;
        
        void bind() const override;
        
        const Size& size() const;
        bool isComplete();
        void attachTexture(const GLDepthTexture2D& texture);
    };
    
}

#endif /* GLFramebuffer_hpp */
