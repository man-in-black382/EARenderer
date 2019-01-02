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
#include <OpenGL/OpenGL.h>

namespace EARenderer {

    template<typename Vertex>
    class GLVertexArrayBuffer : public GLBuffer<Vertex> {
    public:
        template<template<class...> class ContinuousContainer>
        static auto Create(const ContinuousContainer<Vertex> &data) {
            return GLVertexArrayBuffer<Vertex>(data.data(), data.size());
        }

        GLVertexArrayBuffer(const Vertex *data, uint64_t count)
                : GLBuffer<Vertex>(data, count, GL_ARRAY_BUFFER, GL_STATIC_DRAW) {}
    };

}

#endif /* GLVertexArrayBuffer_hpp */
