//
//  GLTexture2D.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 19.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLTexture2D_hpp
#define GLTexture2D_hpp

#include "GLNamedObject.hpp"
#include "GLBindable.hpp"
#include "Size2D.hpp"

namespace EARenderer {
    
    class GLTexture2D: public GLNamedObject, public GLBindable {
    private:
        Size2D mSize;
        
        void applyParameters();
        
    public:
        using GLNamedObject::GLNamedObject;
        
        GLTexture2D(const Size2D& size);
        GLTexture2D(const std::string& fileName);
        
        GLTexture2D(GLTexture2D&& that) = default;
        GLTexture2D& operator=(GLTexture2D&& rhs) = default;
        ~GLTexture2D() override;
        
        void bind() const override;
        
        const Size2D& size() const;
    };
    
}

#endif /* GLTexture2D_hpp */
