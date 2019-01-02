//
// Created by Pavlo Muratov on 2018-12-24.
// Copyright (c) 2018 MPO. All rights reserved.
//

#ifndef EARENDERER_GLUNIFORMBUFFER_HPP
#define EARENDERER_GLUNIFORMBUFFER_HPP

namespace EARenderer {

    // UBO Correct usage and performance
    // https://www.gamedev.net/forums/topic/655969-speed-gluniform-vs-uniform-buffer-objects/?do=findComment&comment=5149918

    template<typename DataType>
    class GLUniformBuffer : public GLBuffer<DataType> {
    private:
        // UBO std140 alignment
        // https://stackoverflow.com/a/38172697/4308277
        // https://www.opengl.org/discussion_boards/showthread.php/175410-Uniform-Buffers-and-BindBufferRange?p=1225853&viewfull=1#post1225853
        GLint mRequiredAlignment;

    public:
        GLUniformBuffer() : GLBuffer<DataType>(GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW) {
            glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &mRequiredAlignment);
        }
    };

}

#endif //EARENDERER_GLUNIFORMBUFFER_HPP
