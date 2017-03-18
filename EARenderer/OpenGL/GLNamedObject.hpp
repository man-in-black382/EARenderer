//
//  GLNamedObject.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLNamedObject_hpp
#define GLNamedObject_hpp

#include <OpenGL/gltypes.h>
#include <iostream>

namespace EARenderer {
    
    class GLNamedObject {
    protected:
        GLuint mName = 0;
        
    public:
        GLNamedObject() = default;
        GLNamedObject(GLuint);
        GLNamedObject(GLNamedObject&&) = default;
        GLNamedObject(const GLNamedObject&) = default;
        GLNamedObject& operator=(const GLNamedObject&);
        virtual ~GLNamedObject() = 0;
        void swap(GLNamedObject&);
        
        GLuint getName() const;
    };
    
    void swap(GLNamedObject&, GLNamedObject&);
}

#endif /* GLNamedObject_hpp */
