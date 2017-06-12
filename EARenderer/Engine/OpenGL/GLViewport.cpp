//
//  GLViewport.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLViewport.hpp"

#include <OpenGL/gl3.h>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLViewport& GLViewport::main() {
        static GLViewport main;
        return main;
    }
    
    GLViewport::GLViewport()
    :
    mRect(Rect::zero())
    { }
    
    GLViewport::GLViewport(const Rect& rect)
    :
    mRect(rect)
    { }
    
#pragma mark - Getters
    
    const Rect& GLViewport::rect() const {
        return mRect;
    }
    
#pragma mark - Setters
    
    void GLViewport::setRect(const Rect& rect) {
        mRect = rect;
    }
    
    void GLViewport::setDimensions(const Size& dimensions) {
        mRect.size = dimensions;
    }
    
#pragma mark - Other methods
    
    void GLViewport::apply() {
        glViewport(mRect.origin.x, mRect.origin.y, mRect.size.width, mRect.size.height);
    }
    
}
