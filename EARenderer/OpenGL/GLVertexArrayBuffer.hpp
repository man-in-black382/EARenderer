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
#include <OpenGL/gl.h>

namespace EARenderer {
    
    template <typename Vertex>
    class GLVertexArrayBuffer : public GLBuffer<Vertex> {
    public:
        void bind() override {
            glBindBuffer(GL_VERTEX_ARRAY, GLBuffer<Vertex>::mName);
        }
        
        void initialize(const std::vector<Vertex>& data) override {
            glBufferData(GL_VERTEX_ARRAY, data.size() * sizeof(Vertex), &data[0], GL_STATIC_DRAW);
        }
    };
    
}

#endif /* GLVertexArrayBuffer_hpp */
