//
//  GLRenderbuffer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 10.12.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLRenderbuffer_hpp
#define GLRenderbuffer_hpp

#include "GLNamedObject.hpp"
#include "Size2D.hpp"

namespace EARenderer {

    class GLRenderbuffer : public GLNamedObject {
    private:
        Size2D mSize;

    protected:
        GLRenderbuffer(const Size2D &size, GLenum internalFormat);

    public:
        GLRenderbuffer(GLRenderbuffer &&that) = default;

        GLRenderbuffer &operator=(GLRenderbuffer &&rhs) = default;

        ~GLRenderbuffer() = 0;

        void bind() const;

        const Size2D &size() const;
    };

}

#endif /* GLRenderbuffer_hpp */
