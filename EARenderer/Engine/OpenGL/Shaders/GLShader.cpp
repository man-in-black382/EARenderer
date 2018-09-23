//
//  GLShader.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLShader.hpp"
#include "StringUtils.hpp"

#include <fstream>
#include <vector>
 

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLShader::GLShader(const std::string& sourcePath, GLenum type) {
        std::ifstream stream(sourcePath);
        if (!stream.is_open()) {
            throw std::invalid_argument(string_format("Can't load a shader at path: %s", sourcePath.c_str()));
        }
        mName = glCreateShader(type);
        compile(std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>()));
    }
    
    GLShader::~GLShader() {
        glDeleteShader(mName);
    }

#pragma mark - Private helper methods
    
    void GLShader::compile(const std::string& source) {
        const char* cStr = source.c_str();
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
                throw std::runtime_error(string_format("Failed to compile shader: %s", infoLog.c_str()));
            }
            
            throw std::runtime_error("Failed to compile shader");
        }
    }
    
#pragma mark - Swap
    
    void GLShader::swap(GLShader& that) {
        GLNamedObject::swap(that);
    }
    
    void swap(GLShader& lhs, GLShader &rhs) {
        lhs.swap(rhs);
    }

}
