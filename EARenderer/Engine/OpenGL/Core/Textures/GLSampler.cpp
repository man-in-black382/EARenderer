//
//  GLSampler.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSampler.hpp"

#include <OpenGL/OpenGL.h>

namespace EARenderer {

#pragma mark - Lifecycle

    GLSampler::GLSampler(Sampling::Filter filter, Sampling::WrapMode wrapMode, Sampling::ComparisonMode comparisonMode) {
        glGenSamplers(1, &mName);
        setFilter(filter);
        setWrapMode(wrapMode);
        setComparisonMode(comparisonMode);
    }

    GLSampler::~GLSampler() {
        glDeleteSamplers(1, &mName);
    }

#pragma mark - Binding

    void GLSampler::setFilter(Sampling::Filter filter) {
        GLint glMinFilter = 0;
        GLint glMagFilter = 0;

        switch (filter) {
            case Sampling::Filter::None:
                glMinFilter = glMagFilter = GL_NEAREST;
                break;
            case Sampling::Filter::Bilinear:
                glMinFilter = glMagFilter = GL_LINEAR;
                break;
            case Sampling::Filter::Trilinear:
                glMinFilter = GL_LINEAR_MIPMAP_LINEAR;
                glMagFilter = GL_LINEAR;
                break;
            case Sampling::Filter::Anisotropic:
                glMinFilter = GL_LINEAR_MIPMAP_LINEAR;
                glMagFilter = GL_LINEAR;
                float aniso = 0.0f;
                glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
                glSamplerParameterf(mName, GL_TEXTURE_MAX_ANISOTROPY_EXT, std::min(aniso, 8.0f));
                break;
        }

        glSamplerParameteri(mName, GL_TEXTURE_MIN_FILTER, glMinFilter);
        glSamplerParameteri(mName, GL_TEXTURE_MAG_FILTER, glMagFilter);
    }

    void GLSampler::setWrapMode(Sampling::WrapMode wrapMode) {
        GLint wrap = 0;

        switch (wrapMode) {
            case Sampling::WrapMode::Repeat:
                wrap = GL_REPEAT;
                break;
            case Sampling::WrapMode::ClampToEdge:
                wrap = GL_CLAMP_TO_EDGE;
                break;
            case Sampling::WrapMode::ClampToBorder:
                wrap = GL_CLAMP_TO_BORDER;
                break;
        }

        glSamplerParameteri(mName, GL_TEXTURE_WRAP_S, wrap);
        glSamplerParameteri(mName, GL_TEXTURE_WRAP_T, wrap);
        glSamplerParameteri(mName, GL_TEXTURE_WRAP_R, wrap);
    }

    void GLSampler::setComparisonMode(Sampling::ComparisonMode comparisonMode) {
        switch (comparisonMode) {
            case Sampling::ComparisonMode::None:
                glSamplerParameteri(mName, GL_TEXTURE_COMPARE_MODE, GL_NONE);
                break;
            case Sampling::ComparisonMode::ReferenceToTexture:
                glSamplerParameteri(mName, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                glSamplerParameteri(mName, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
                break;
        }
    }

}
