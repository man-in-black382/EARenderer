//
//  GLDepthTextureCubemap.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLDepthTextureCubemap_hpp
#define GLDepthTextureCubemap_hpp

#include "GLNamedObject.hpp"
#include "GLBindable.hpp"
#include "Size2D.hpp"

namespace EARenderer {
    
    class GLDepthTextureCubemap: public GLNamedObject, public GLBindable {
    private:
        Size2D mSize;
        
        void applyParameters();
        
    public:
        using GLNamedObject::GLNamedObject;
        GLDepthTextureCubemap(const Size2D& size);
        GLDepthTextureCubemap(GLDepthTextureCubemap&& that) = default;
        GLDepthTextureCubemap& operator=(GLDepthTextureCubemap&& rhs) = default;
        ~GLDepthTextureCubemap() override;
        
        const Size2D& size() const;
        
        void bind() const override;
    };
    
}

#endif /* GLDepthTextureCubemap_hpp */
