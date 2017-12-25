//
//  GLVertexArrayBuffer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLVertexArrayBuffer_hpp
#define GLVertexArrayBuffer_hpp

#include "GLBuffer.hpp"
#include <OpenGL/gl3.h>

namespace EARenderer {
    
    template <typename Vertex>
    class GLVertexArrayBuffer : public GLBuffer<Vertex> {
    public:
        GLVertexArrayBuffer()
        :
        GLBuffer<Vertex>(GL_ARRAY_BUFFER, GL_STATIC_DRAW)
        { }
    };
    
}

#endif /* GLVertexArrayBuffer_hpp */
