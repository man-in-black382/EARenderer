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
#include <string>
#include <vector>
#include <array>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLNamedObject.hpp"
#include "GLShader.hpp"
#include "GLBindable.hpp"
#include "GLTexture2D.hpp"
#include "GLTextureCubemap.hpp"
#include "GLDepthTexture2D.hpp"
#include "GLDepthTextureCubemap.hpp"
#include "GLDepthTexture2DArray.hpp"

namespace EARenderer {
    
    class GLProgram: public GLNamedObject, public GLBindable {
    private:
        const GLShader* mVertexShader = nullptr;
        const GLShader* mFragmentShader = nullptr;
        const GLShader* mGeometryShader = nullptr;
        std::unordered_map<std::string, GLint> mUniforms;
        GLint mAvailableTextureUnits = 0;
        GLint mFreeTextureUnitIndex = 0;
        
        void link();
        void obtainUniforms();
        void obtainAvailableTextureUnits();
        void setUniformTexture(const std::string& uniformName, const GLBindable& texture);
        
    protected:
        GLProgram(const std::string& vertexSourceName, const std::string& fragmentSourceName, const std::string& geometrySourceName);
        
        GLint uniformLocation(const std::string& name);
        void setUniformTexture(const std::string& uniformName, const GLTexture2D& texture);
        void setUniformTexture(const std::string& uniformName, const GLTextureCubemap& texture);
        void setUniformTexture(const std::string& uniformName, const GLDepthTexture2D& texture);
        void setUniformTexture(const std::string& uniformName, const GLDepthTextureCubemap& texture);
        void setUniformTexture(const std::string& uniformName, const GLDepthTexture2DArray& texture);
        
        template <std::size_t size>
        void setUniformArray(const std::string& uniformName, const std::array<glm::mat4, size>& matrices) {
            for(int i = 0; i < matrices.size(); ++i) {
                glUniformMatrix4fv(uniformLocation(uniformName + "[" + std::to_string(i) + "]"), 1, GL_FALSE, glm::value_ptr(matrices[i]));
            }
        }
        
    public:
        GLProgram(const GLProgram& rhs) = delete;
        GLProgram& operator=(const GLProgram& that) = delete;
        GLProgram(GLProgram&& that) = default;
        GLProgram& operator=(GLProgram&& that) = default;
        virtual ~GLProgram() = 0;
        void swap(GLProgram&);
        
        void bind() const override;
        
        void flushState();
    };
    
    void swap(GLProgram&, GLProgram&);
    
}

#endif /* GLProgram_hpp */
