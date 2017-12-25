//
//  GLElementArrayBuffer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLElementArrayBuffer.hpp"

namespace EARenderer {
    
    GLElementArrayBuffer::GLElementArrayBuffer()
    :
    GLBuffer<GLushort>(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW)
    { }
    
}
