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
        link();
        bind();
        obtainAvailableTextureUnits();
        obtainUniforms();
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
                glGetProgramInfoLog(mName, infoLength, nullptr, infoChars.data());
                std::string infoLog(infoChars.data());
                ASSERT(isLinked, "Failed to link program: " << infoLog);
            }
            
            ASSERT(isLinked, "Failed to link program");
        }
    }
    
    void GLProgram::obtainUniforms() {
        GLint count = 0;
        glGetProgramiv(mName, GL_ACTIVE_UNIFORMS, &count);
        
        GLuint textureUnit = 0;
        for (GLuint index = 0; index < count; index++) {
            std::vector<GLchar> uniformNameChars(128);
            GLint size;
            GLenum type;
            glGetActiveUniform(mName, index, static_cast<GLsizei>(uniformNameChars.size()), nullptr, &size, &type, uniformNameChars.data());
            std::string name(uniformNameChars.data());
            GLint location = glGetUniformLocation(mName, &name[0]);
    
            GLUniform uniform = GLUniform(location, size, type, name);
            
            if (uniform.isSampler()) {
                ASSERT(textureUnit < mAvailableTextureUnits, "Exceeded the number of available texture units (" << mAvailableTextureUnits << ")");
                glUniform1i(uniform.location(), textureUnit);
                uniform.setTextureUnit(textureUnit);
                textureUnit++;
            }
            
            uint32_t crc = crc32(name.c_str(), name.length());
            mUniforms.insert(std::make_pair(crc, uniform));
        }
    }
    
    void GLProgram::obtainAvailableTextureUnits() {
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &mAvailableTextureUnits);
    }
    
#pragma mark - Swap
    
    void GLProgram::swap(GLProgram& that) {
        GLNamedObject::swap(that);
        std::swap(mVertexShader, that.mVertexShader);
        std::swap(mFragmentShader, that.mFragmentShader);
        std::swap(mGeometryShader, that.mGeometryShader);
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
    
    void GLProgram::setUniformTexture(uint32_t uniformNameCRC32, const GLTexture& texture) {
        GLUniform sampler = uniformByNameCRC32(uniformNameCRC32);
        ASSERT(isModifyingUniforms, "You must set texture/sampler uniforms inside a designated closure provided by 'modifyUniforms' member fuction");
        glActiveTexture(GL_TEXTURE0 + sampler.textureUnit());
        texture.bind();
    }
      
    const GLUniform& GLProgram::uniformByNameCRC32(uint32_t crc32) {
        auto it = mUniforms.find(crc32);
        ASSERT(it != mUniforms.end(), "Uniform could not be found");
        return it->second;
    }
    
#pragma mark - Public
    
    void GLProgram::ensureSamplerValidity(UniformModifierClosure closure) {
        isModifyingUniforms = true;
        closure();

        // For some reason this is sometimes necessary to make GLSL sampler not to return black
        // (in a case of radiance convolution shader sampling cubemap gives black color without following line)
        glActiveTexture(GL_TEXTURE0 + mAvailableTextureUnits - 1);
        isModifyingUniforms = false;
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
