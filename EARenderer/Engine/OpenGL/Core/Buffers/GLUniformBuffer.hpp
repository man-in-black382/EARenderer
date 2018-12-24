//
// Created by Pavlo Muratov on 2018-12-24.
// Copyright (c) 2018 MPO. All rights reserved.
//

#ifndef EARENDERER_GLUNIFORMBUFFER_HPP
#define EARENDERER_GLUNIFORMBUFFER_HPP

namespace EARenderer {

    // UBO Correct usage and performance
    // https://www.gamedev.net/forums/topic/655969-speed-gluniform-vs-uniform-buffer-objects/?do=findComment&comment=5149918
    //
    template <typename DataType>
    class GLUniformBuffer : public GLBuffer<DataType> {
    public:
        GLUniformBuffer() : GLBuffer<DataType>(GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW) {}
    };

}

#endif //EARENDERER_GLUNIFORMBUFFER_HPP
