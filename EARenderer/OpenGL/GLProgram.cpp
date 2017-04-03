//
//  GLProgram.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLProgram.hpp"

#include <vector>
#include <sstream>
#include <OpenGL/gl.h>

#include "Macros.h"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLProgram::GLProgram(const GLShader* vertexShader, const GLShader* fragmentShader)
    :
    GLNamedObject(glCreateProgram()),
    mVertexShader(vertexShader),
    mFragmentShader(fragmentShader)
    {
        link();
        obtainUniforms();
    }
    
    GLProgram::~GLProgram() {
        glDeleteProgram(mName);
    }
    
#pragma mark - Private helper methods
    
    void GLProgram::link() {
        glAttachShader(mName, mVertexShader->getName());
        glAttachShader(mName, mFragmentShader->getName());
        glLinkProgram(mName);
        
        GLint isLinked = 0;
        glGetProgramiv(getName(), GL_LINK_STATUS, &isLinked);
        
        if (!isLinked) {
            GLint infoLength = 0;
            glGetProgramiv(getName(), GL_INFO_LOG_LENGTH, &infoLength);
            
            if (infoLength > 1) {
                std::vector<char> infoChars(infoLength);
                glGetShaderInfoLog(getName(), infoLength, nullptr, infoChars.data());
                std::string infoLog(infoChars.begin(), infoChars.end());
                ASSERT(isLinked, "Failed to link program: " << infoLog);
            }
            
            ASSERT(isLinked, "Failed to link program");
        }
    }
    
    void GLProgram::obtainUniforms() {
        GLint count = 0;
        glGetProgramiv(mName, GL_ACTIVE_UNIFORMS, &count);
        
        for (GLuint index = 0; index < count; index++) {
            std::vector<GLchar> uniformNameChars(128);
            glGetActiveUniform(mName, index, static_cast<GLsizei>(uniformNameChars.size()), nullptr, nullptr, nullptr, uniformNameChars.data());
            std::string name(uniformNameChars.begin(), uniformNameChars.end());
            mUniforms[name] = glGetUniformLocation(mName, &name[0]);
        }
    }
    
#pragma mark - Swap
    
    void GLProgram::swap(GLProgram& that) {
        GLNamedObject::swap(that);
        std::swap(mVertexShader, that.mVertexShader);
        std::swap(mFragmentShader, that.mFragmentShader);
        std::swap(mUniforms, that.mUniforms);
    }
    
    void swap(GLProgram& lhs, GLProgram& rhs) {
        lhs.swap(rhs);
    }
    
#pragma mark - Bindable
    
    void GLProgram::bind() {
        glUseProgram(mName);
    }
    
#pragma mark - Public
    
    GLint GLProgram::uniformLocation(const std::string& name) {
        return mUniforms[name];
    }
    
}
