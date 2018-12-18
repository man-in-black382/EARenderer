//
//  GLNamedObject.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLNamedObject_hpp
#define GLNamedObject_hpp

#include <OpenGL/gl3.h>
#include <iostream>

namespace EARenderer {

    class GLNamedObject {
    protected:
        GLuint mName = -1;

    public:
        GLNamedObject() = default;

        GLNamedObject(GLuint);

        GLNamedObject(GLNamedObject &&);

        GLNamedObject &operator=(GLNamedObject &&);

        GLNamedObject(const GLNamedObject &) = delete;

        GLNamedObject &operator=(const GLNamedObject &) = delete;

        virtual ~GLNamedObject() = 0;

        void swap(GLNamedObject &);

        GLuint name() const;
    };

    void swap(GLNamedObject &, GLNamedObject &);
}

#endif /* GLNamedObject_hpp */
