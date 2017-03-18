//
//  GLShader.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLShader.hpp"

#include <fstream>
#include <vector>
#include <OpenGL/gl.h>
#include "Macros.h"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLShader::GLShader(const std::string& sourcePath, GLShaderType type)
    :
    GLNamedObject(0),
    mType(type)
    {
        std::ifstream stream(sourcePath);
        ASSERT(stream.is_open(), "Cannot load shader (" << sourcePath << ")");
        mSource = std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        
        GLenum glType;
        switch (type) {
            case GLShaderType::vertex: glType = GL_VERTEX_SHADER; break;
            case GLShaderType::fragment: glType = GL_FRAGMENT_SHADER; break;
        }
        
        mName = glCreateShader(glType);
        
        compile();
    }
    
    GLShader::~GLShader() {
        glDeleteShader(mName);
    }
    
#pragma mark - Operators
    
    GLShader& GLShader::operator=(GLShader that) {
        GLNamedObject::operator=(that);
        swap(that);
        return *this;
    }
    
#pragma mark - Private helper methods
    
    void GLShader::compile() {
        const char* cStr = mSource.c_str();
        glShaderSource(mName, 1, &cStr, nullptr);
        glCompileShader(mName);
        
        GLint isCompiled = 0;
        glGetShaderiv(mName, GL_COMPILE_STATUS, &isCompiled);
        
        if (!isCompiled) {
            GLint infoLength = 0;
            glGetShaderiv(mName, GL_INFO_LOG_LENGTH, &infoLength);
            
            if (infoLength > 1) {
                std::vector<char> infoChars(infoLength);
                glGetShaderInfoLog(mName, infoLength, nullptr, infoChars.data());
                std::string infoLog(infoChars.begin(), infoChars.end());
                ASSERT(isCompiled, "Falied to compile shader: " << infoLog);
            }
            
            ASSERT(isCompiled, "Falied to compile shader");
        }
    }
    
#pragma mark - Swap
    
    void GLShader::swap(GLShader& that) {
        GLNamedObject::swap(that);
        std::swap(mType, that.mType);
        std::swap(mSource, that.mSource);
    }
    
    void swap(GLShader& lhs, GLShader &rhs) {
        lhs.swap(rhs);
    }
    
#pragma mark - Accessors
    
    const std::string& GLShader::getSource() {
        return mSource;
    }
    
    GLShaderType GLShader::getType() {
        return mType;
    }
}
