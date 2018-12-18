//
//  GLUniform.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 10.09.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLUniform.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLUniform::GLUniform(GLint location, GLint size, GLenum type, const std::string &name)
            :
            GLUniform(location, size, type, -1, name) {
    }

    GLUniform::GLUniform(GLint location, GLint size, GLenum type, GLint textureUnit, const std::string &name)
            :
            mLocation(location),
            mSize(size),
            mType(type),
            mTextureUnit(textureUnit),
            mName(name) {
    }

#pragma mark - Getters

    GLint GLUniform::location() const {
        return mLocation;
    }

    GLint GLUniform::size() const {
        return mSize;
    }

    GLenum GLUniform::type() const {
        return mType;
    }

    GLint GLUniform::textureUnit() const {
        return mTextureUnit;
    }

    const std::string &GLUniform::name() const {
        return mName;
    }

#pragma mark - Setters

    void GLUniform::setTextureUnit(GLint unit) {
        mTextureUnit = unit;
    }

#pragma mark - Utility

    bool GLUniform::isValid() const {
        return mLocation != -1;
    }

    bool GLUniform::isSampler() const {
        return
                mType == GL_SAMPLER_1D
                        || mType == GL_SAMPLER_2D
                        || mType == GL_SAMPLER_3D
                        || mType == GL_SAMPLER_CUBE
                        || mType == GL_SAMPLER_1D_SHADOW
                        || mType == GL_SAMPLER_2D_SHADOW
                        || mType == GL_SAMPLER_1D_ARRAY
                        || mType == GL_SAMPLER_2D_ARRAY
                        || mType == GL_SAMPLER_1D_ARRAY_SHADOW
                        || mType == GL_SAMPLER_2D_ARRAY_SHADOW
                        || mType == GL_SAMPLER_2D_MULTISAMPLE
                        || mType == GL_SAMPLER_2D_MULTISAMPLE_ARRAY
                        || mType == GL_SAMPLER_CUBE_SHADOW
                        || mType == GL_SAMPLER_BUFFER
                        || mType == GL_SAMPLER_2D_RECT
                        || mType == GL_SAMPLER_2D_RECT_SHADOW
                        || mType == GL_INT_SAMPLER_1D
                        || mType == GL_INT_SAMPLER_2D
                        || mType == GL_INT_SAMPLER_3D
                        || mType == GL_INT_SAMPLER_CUBE
                        || mType == GL_INT_SAMPLER_1D_ARRAY
                        || mType == GL_INT_SAMPLER_2D_ARRAY
                        || mType == GL_INT_SAMPLER_2D_MULTISAMPLE
                        || mType == GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
                        || mType == GL_INT_SAMPLER_BUFFER
                        || mType == GL_INT_SAMPLER_2D_RECT
                        || mType == GL_UNSIGNED_INT_SAMPLER_1D
                        || mType == GL_UNSIGNED_INT_SAMPLER_2D
                        || mType == GL_UNSIGNED_INT_SAMPLER_3D
                        || mType == GL_UNSIGNED_INT_SAMPLER_CUBE
                        || mType == GL_UNSIGNED_INT_SAMPLER_1D_ARRAY
                        || mType == GL_UNSIGNED_INT_SAMPLER_2D_ARRAY
                        || mType == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE
                        || mType == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
                        || mType == GL_UNSIGNED_INT_SAMPLER_BUFFER
                        || mType == GL_UNSIGNED_INT_SAMPLER_2D_RECT
                        || mType == GL_SAMPLER_CUBE_MAP_ARRAY
                        || mType == GL_INT_SAMPLER_CUBE_MAP_ARRAY
                        || mType == GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY;
    }

}
