//
//  GLProgram.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLProgram.hpp"
#include "FileManager.hpp"
#include "Macros.h"

#include <vector>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLProgram::GLProgram(const std::string& vertexSourceName, const std::string& fragmentSourceName, const std::string& geometrySourceName)
    :
    GLNamedObject(glCreateProgram()),
    mVertexShader(vertexSourceName.empty() ? nullptr : new GLShader(FileManager::shared().shaderSourceFolderPath() + vertexSourceName, GL_VERTEX_SHADER)),
    mFragmentShader(fragmentSourceName.empty() ? nullptr : new GLShader(FileManager::shared().shaderSourceFolderPath() + fragmentSourceName, GL_FRAGMENT_SHADER)),
    mGeometryShader(geometrySourceName.empty() ? nullptr : new GLShader(FileManager::shared().shaderSourceFolderPath() + geometrySourceName, GL_GEOMETRY_SHADER))
    {
        printf("Vertex source: %s\n", vertexSourceName.c_str());
        
        link();
        obtainUniforms();
        obtainAvailableTextureUnits();
    }
    
    GLProgram::~GLProgram() {
        glDeleteProgram(mName);
        
        delete mVertexShader;
        delete mGeometryShader;
        delete mFragmentShader;
    }
    
#pragma mark - Private helper methods
    
    void GLProgram::link() {
        glAttachShader(mName, mVertexShader->name());
        glAttachShader(mName, mFragmentShader->name());
        
        if (mGeometryShader) { glAttachShader(mName, mGeometryShader->name()); }
        
        glLinkProgram(mName);
        
        GLint isLinked = 0;
        glGetProgramiv(mName, GL_LINK_STATUS, &isLinked);
        
        if (!isLinked) {
            GLint infoLength = 0;
            glGetProgramiv(mName, GL_INFO_LOG_LENGTH, &infoLength);
            
            if (infoLength > 1) {
                std::vector<char> infoChars(infoLength);
                glGetShaderInfoLog(mName, infoLength, nullptr, infoChars.data());
                std::string infoLog(infoChars.data());
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
            std::string name(uniformNameChars.data());
            // Increment by 1 to use 0 as a non-used uniform indicator
            mUniforms[name] = glGetUniformLocation(mName, &name[0]) + 1;
        }
    }
    
    void GLProgram::obtainAvailableTextureUnits() {
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &mAvailableTextureUnits);
    }
    
    void GLProgram::setUniformTexture(const std::string& uniformName, const GLBindable& texture) {
        GLint location = uniformLocation(uniformName);
        ASSERT(mFreeTextureUnitIndex < mAvailableTextureUnits, "Exceeded the number of available texture units (" << mAvailableTextureUnits << ")");
        glActiveTexture(GL_TEXTURE0 + mFreeTextureUnitIndex);
        glUniform1i(location, mFreeTextureUnitIndex);
        texture.bind();
        mFreeTextureUnitIndex++;
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
    
    void GLProgram::bind() const {
        glUseProgram(mName);
    }
    
#pragma mark - Protected
    
    void GLProgram::setUniformTexture(const std::string& uniformName, const GLTexture2D& texture) {
        setUniformTexture(uniformName, dynamic_cast<const GLBindable&>(texture));
    }
    
    void GLProgram::setUniformTexture(const std::string& uniformName, const GLTextureCubemap& texture) {
        setUniformTexture(uniformName, dynamic_cast<const GLBindable&>(texture));
    }
    
    void GLProgram::setUniformTexture(const std::string& uniformName, const GLDepthTexture2D& texture) {
        setUniformTexture(uniformName, dynamic_cast<const GLBindable&>(texture));
    }
    
    void GLProgram::setUniformTexture(const std::string& uniformName, const GLDepthTextureCubemap& texture) {
        setUniformTexture(uniformName, dynamic_cast<const GLBindable&>(texture));
    }
    
    void GLProgram::setUniformTexture(const std::string& uniformName, const GLDepthTexture2DArray& texture) {
        setUniformTexture(uniformName, dynamic_cast<const GLBindable&>(texture));
    }
    
    void GLProgram::setUniformTexture(const std::string& uniformName, const GLTexture2DArray& texture) {
        setUniformTexture(uniformName, dynamic_cast<const GLBindable&>(texture));
    }
    
    GLint GLProgram::uniformLocation(const std::string& name) {
        // Decrement location to return -1 for non-used uniform
        return mUniforms[name] - 1;
    }
    
#pragma mark - Public
    
    void GLProgram::flushState() {
        mFreeTextureUnitIndex = 0;
    }
    
    bool GLProgram::validateState() const {
        GLsizei loglen = 0;
        GLchar logbuffer[1000];
        glValidateProgram(mName);
        glGetProgramInfoLog(mName, sizeof(logbuffer), &loglen, logbuffer);
        if (loglen > 0) {
            printf("OpenGL Program %d is invalid: \n %s\n", mName, logbuffer);
            return false;
        }
        return true;
    }
    
}
