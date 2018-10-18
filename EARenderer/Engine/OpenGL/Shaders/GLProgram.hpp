//
//  GLProgram.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLProgram_hpp
#define GLProgram_hpp

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <array>
#include <type_traits>
#include <stdexcept>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLNamedObject.hpp"
#include "GLVertexAttribute.hpp"
#include "GLUniform.hpp"
#include "GLShader.hpp"
#include "GLBindable.hpp"
#include "GLTexture2D.hpp"
#include "GLTextureCubemap.hpp"
#include "GLTexture2D.hpp"
#include "GLDepthTextureCubemap.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "GLTexture2DArray.hpp"
#include "GLBufferTexture.hpp"
#include "crc.hpp"

namespace EARenderer {
    
    template<uint32_t expr>
    std::integral_constant<uint32_t, expr> uint32_constant{};
    
    class GLProgram: public GLNamedObject, public GLBindable {
    protected:
        using VertexAttributeName = std::string;
        using CRC32 = uint32_t;

    private:
        const GLShader* mVertexShader = nullptr;
        const GLShader* mFragmentShader = nullptr;
        const GLShader* mGeometryShader = nullptr;

        std::unordered_map<VertexAttributeName, GLVertexAttribute> mVertexAttributes;
        std::unordered_map<CRC32, GLUniform> mUniforms;
        
        GLint mAvailableTextureUnits = 0;
        
        bool isModifyingUniforms = false;

        void link();
        void obtainVertexAttributes();
        void obtainUniforms();
        void obtainAvailableTextureUnits();
        
    protected:
        GLProgram(const std::string& vertexSourceName, const std::string& fragmentSourceName, const std::string& geometrySourceName);

        void setUniformTexture(CRC32 uniformNameCRC32, const GLTexture& texture);
        
        template <typename BufferDataType>
        void setUniformTexture(CRC32 uniformNameCRC32, const GLBufferTexture<BufferDataType>& bufferTexture) {
            if (bufferTexture.buffer().size() == 0) {
                throw std::runtime_error("Passing empty texture buffer to a uniform is not allowed");
            }

            GLUniform sampler = uniformByNameCRC32(uniformNameCRC32);
            glActiveTexture(GL_TEXTURE0 + sampler.textureUnit());
            bufferTexture.bind();
        }
        
    public:
        using UniformModifierClosure = const std::function<void()>&;
        
        GLProgram(const GLProgram& rhs) = delete;
        GLProgram& operator=(const GLProgram& that) = delete;
        GLProgram(GLProgram&& that) = default;
        GLProgram& operator=(GLProgram&& that) = default;
        virtual ~GLProgram() = 0;
        void swap(GLProgram&);
        
        void bind() const override;
        bool validateState() const;

        const GLVertexAttribute& vertexAttributeByName(const std::string& name);
        const GLUniform& uniformByNameCRC32(CRC32 crc32);
        
        /**
         You should use this function for setting up uniform sampler values.
         If GLSL code for a particular program requires optional samplers,
         (no normal or gloss map provided for a particular object, etc.)
         it ensures 1-to-1 sampler - texture unit mapping for all unused samplers

         @param closure function object in which uniform sampler modifications should be performed
         */
        void ensureSamplerValidity(UniformModifierClosure closure);
    };
    
    void swap(GLProgram&, GLProgram&);
    
}

#endif /* GLProgram_hpp */
