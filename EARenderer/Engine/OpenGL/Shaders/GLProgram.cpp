//
//  GLProgram.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLProgram.hpp"
#include "FileManager.hpp"
#include "StringUtils.hpp"

#include <sstream>

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLProgram::GLProgram(const std::string& vertexSourceName, const std::string& fragmentSourceName, const std::string& geometrySourceName)
    :
    GLNamedObject(glCreateProgram()),
    mVertexShader(vertexSourceName.empty() ? nullptr : new GLShader(FileManager::shared().resourceRootPath() + vertexSourceName, GL_VERTEX_SHADER)),
    mFragmentShader(fragmentSourceName.empty() ? nullptr : new GLShader(FileManager::shared().resourceRootPath() + fragmentSourceName, GL_FRAGMENT_SHADER)),
    mGeometryShader(geometrySourceName.empty() ? nullptr : new GLShader(FileManager::shared().resourceRootPath() + geometrySourceName, GL_GEOMETRY_SHADER))
    {
        link();
        bind();
        obtainAvailableTextureUnits();
        obtainVertexAttributes();
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

                if (!isLinked) {
                    throw std::runtime_error(string_format("Failed to link program: %s", infoLog.c_str()));
                }
            }
            
            if (!isLinked) {
                throw std::runtime_error("Failed to link program");
            }
        }
    }

    void GLProgram::obtainVertexAttributes() {
        GLint count = 0;
        glGetProgramiv(mName, GL_ACTIVE_ATTRIBUTES, &count);

        for (GLuint index = 0; index < count; index++) {
            std::vector<GLchar> attributeNameChars(128);
            GLint size;
            GLenum type;
            glGetActiveAttrib(mName, index, static_cast<GLsizei>(attributeNameChars.size()), nullptr, &size, &type, attributeNameChars.data());
            std::string name(attributeNameChars.data());
//            GLint location = glGetAttribLocation(mName, &name[0]);
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
                if (textureUnit >= mAvailableTextureUnits) {
                    throw std::runtime_error(string_format("Exceeded the number of available texture units (%d)", mAvailableTextureUnits));
                }
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
    
    void GLProgram::setUniformTexture(CRC32 uniformNameCRC32, const GLTexture& texture) {
        GLUniform sampler = uniformByNameCRC32(uniformNameCRC32);

        if (!sampler.isSampler()) {
            throw std::runtime_error(string_format("Uniform %s is not a sampler", sampler.name().c_str()));
        }

        if (!isModifyingUniforms) {
            throw std::logic_error("You must set texture/sampler uniforms inside a designated closure provided by 'modifyUniforms' member fuction");
        }

        glActiveTexture(GL_TEXTURE0 + sampler.textureUnit());
        texture.bind();
    }
    
#pragma mark - Public

    const GLVertexAttribute& GLProgram::vertexAttributeByName(const std::string& name) {
        auto it = mVertexAttributes.find(name);
        if (it == mVertexAttributes.end()) {
            throw std::invalid_argument(string_format("Vertex attribute '%s' couldn't be found", name.c_str()));
        }
        return it->second;
    }

    const GLUniform& GLProgram::uniformByNameCRC32(CRC32 crc32) {
        auto it = mUniforms.find(crc32);
        if (it == mUniforms.end()) {
            throw std::invalid_argument("Uniform couldn't be found");
        }
        return it->second;
    }

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
