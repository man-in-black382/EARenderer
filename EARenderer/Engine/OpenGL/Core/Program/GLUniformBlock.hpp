//
// Created by Pavlo Muratov on 2018-12-24.
// Copyright (c) 2018 MPO. All rights reserved.
//

#ifndef EARENDERER_GLUNIFORMBLOCK_HPP
#define EARENDERER_GLUNIFORMBLOCK_HPP

#include <OpenGL/OpenGL.h>
#include <string>

namespace EARenderer {

    class GLUniformBlock {
    private:
        std::string mName; // Name as declared in code
        GLuint mIndex; // Name from a driver standpoint
        GLuint mBinding; // Binding point (like a texture unit for textures)

    public:
        GLUniformBlock(const std::string &name, GLuint index, GLuint binding)
                : mName(name), mIndex(index), mBinding(binding) {}

        const std::string &name() const {
            return mName;
        }

        GLuint index() const {
            return mIndex;
        }

        GLuint binding() const {
            return mBinding;
        }

    };

}

#endif //EARENDERER_GLUNIFORMBLOCK_HPP
