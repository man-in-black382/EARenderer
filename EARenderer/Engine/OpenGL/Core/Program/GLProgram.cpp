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
#include "GLTextureUnitManager.hpp"

#include <sstream>
#include <regex>

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    GLProgram::GLProgram(const std::string &vertexSourceName, const std::string &fragmentSourceName, const std::string &geometrySourceName)
            : GLNamedObject(glCreateProgram()),
              mVertexShader(vertexSourceName.empty() ? nullptr : new GLShader(FileManager::shared().resourceRootPath() + vertexSourceName, GL_VERTEX_SHADER)),
              mFragmentShader(fragmentSourceName.empty() ? nullptr : new GLShader(FileManager::shared().resourceRootPath() + fragmentSourceName, GL_FRAGMENT_SHADER)),
              mGeometryShader(geometrySourceName.empty() ? nullptr : new GLShader(FileManager::shared().resourceRootPath() + geometrySourceName, GL_GEOMETRY_SHADER)) {

        link();
        bind();
        obtainVertexAttributes();
        obtainUniforms();
        obtainUniformBlocks();
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

        if (mFragmentShader) {glAttachShader(mName, mFragmentShader->name());}
        if (mGeometryShader) {glAttachShader(mName, mGeometryShader->name());}

        glLinkProgram(mName);

        GLint isLinked = 0;
        glGetProgramiv(mName, GL_LINK_STATUS, &isLinked);

        if (isLinked) {
            return;
        }

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
                if (textureUnit >= GLTextureUnitManager::Shared().maximumTextureUnits()) {
                    throw std::runtime_error(string_format("Exceeded the number of available texture units (%d)", mAvailableTextureUnits));
                }
                glUniform1i(uniform.location(), textureUnit);
                uniform.setTextureUnit(textureUnit);

                textureUnit++;
            }

            uint32_t crc = ctcrc32(name);
            mUniforms.insert(std::make_pair(crc, uniform));
        }
    }

    void GLProgram::obtainUniformBlocks() {
        GLint numBlocks;
        glGetProgramiv(mName, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);

        GLint maxUBOBindings;
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUBOBindings);

        std::vector<std::string> nameList;
        nameList.reserve(numBlocks);

        for (GLuint blockIndex = 0; blockIndex < numBlocks; ++blockIndex) {
            if (blockIndex >= maxUBOBindings) {
                throw std::range_error(string_format("Exceeded maximum number of Uniform Buffer Object bindings (%d)", maxUBOBindings));
            }

            GLint nameLen;
            glGetActiveUniformBlockiv(mName, blockIndex, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);

            std::vector<GLchar> name;
            name.resize(nameLen);
            glGetActiveUniformBlockName(mName, blockIndex, nameLen, nullptr, &name[0]);

            nameList.emplace_back();
            nameList.back().assign(name.begin(), name.end() - 1); // Remove the null terminator.

            GLuint binding = uniformBlockBinding(nameList.back(), maxUBOBindings);
            glUniformBlockBinding(mName, blockIndex, binding);

            GLUniformBlock block(nameList.back(), blockIndex, binding);
            CRC32 key = ctcrc32(nameList.back());
            mUniformBlocks.insert(std::pair<CRC32, GLUniformBlock>(key, block));

            printf("Program %d | UBO Name %s | UBO Index %d | UBO Binding %d \n", mName, nameList.back().c_str(), blockIndex, binding);
        }
    }

    GLuint GLProgram::uniformBlockBinding(const std::string &UBOName, GLint maximumUBOBindings) {
        CRC32 nameHash = ctcrc32(UBOName);
        return nameHash % maximumUBOBindings;
    }

#pragma mark - Swap

    void GLProgram::swap(GLProgram &that) {
        GLNamedObject::swap(that);
        std::swap(mVertexShader, that.mVertexShader);
        std::swap(mFragmentShader, that.mFragmentShader);
        std::swap(mGeometryShader, that.mGeometryShader);
        std::swap(mUniforms, that.mUniforms);
    }

    void swap(GLProgram &lhs, GLProgram &rhs) {
        lhs.swap(rhs);
    }

#pragma mark - Bindable

    void GLProgram::bind() const {
        glUseProgram(mName);
    }

#pragma mark - Protected

    const GLVertexAttribute &GLProgram::vertexAttributeByName(const std::string &name) {
        auto it = mVertexAttributes.find(name);
        if (it == mVertexAttributes.end()) {
            throw std::invalid_argument(string_format("Vertex attribute '%s' couldn't be found", name.c_str()));
        }
        return it->second;
    }

    const GLUniform &GLProgram::uniformByNameCRC32(CRC32 crc32) {
        auto it = mUniforms.find(crc32);
        if (it == mUniforms.end()) {
            throw std::invalid_argument("Uniform couldn't be found");
        }
        return it->second;
    }

    const GLUniformBlock &GLProgram::uniformBlockByNameCRC32(CRC32 crc32) {
        auto it = mUniformBlocks.find(crc32);
        if (it == mUniformBlocks.end()) {
            throw std::invalid_argument("Uniform block couldn't be found");
        }
        return it->second;
    }

    void GLProgram::setUniformTexture(CRC32 uniformNameCRC32, const GLTexture &texture, const GLSampler *sampler) {
        const GLUniform& uniform = uniformByNameCRC32(uniformNameCRC32);

        if (!uniform.isSampler()) {
            throw std::runtime_error(string_format("Uniform %s is not a texture", uniform.name().c_str()));
        }

        if (!isModifyingUniforms) {
            throw std::logic_error("You must set texture/sampler uniforms inside a designated closure provided by 'ensureSamplerValidity' member function");
        }

        GLTextureUnitManager::Shared().bindTextureToUnit(texture, uniform.textureUnit());

        if (sampler) {
            GLTextureUnitManager::Shared().bindSamplerToUnit(*sampler, uniform.textureUnit());
        }
    }

#pragma mark - Public

    void GLProgram::ensureSamplerValidity(UniformModifierClosure closure) {
        isModifyingUniforms = true;
        GLTextureUnitManager::Shared().unbindAllSamplers();
        closure();

        // For some reason this is sometimes necessary to make GLSL sampler not to return Black
        // (in a case of radiance convolution shader sampling cubemap gives Black color without following line)
        GLTextureUnitManager::Shared().activateUnit(GLTextureUnitManager::Shared().maximumTextureUnits() - 1);
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
