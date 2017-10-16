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

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLNamedObject.hpp"
#include "GLUniform.hpp"
#include "GLShader.hpp"
#include "GLBindable.hpp"
#include "GLTexture2D.hpp"
#include "GLTextureCubemap.hpp"
#include "GLDepthTexture2D.hpp"
#include "GLDepthTextureCubemap.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "GLTexture2DArray.hpp"

namespace EARenderer {
    
    class GLProgram: public GLNamedObject, public GLBindable {
    private:
        const GLShader* mVertexShader = nullptr;
        const GLShader* mFragmentShader = nullptr;
        const GLShader* mGeometryShader = nullptr;
        
        std::unordered_map<std::string, GLUniform> mUniforms;

        GLint mAvailableTextureUnits = 0;
        GLint mFreeTextureUnitIndex = 0;
        bool isModifyingUniforms = false;
        
        void link();
        void obtainUniforms();
        void obtainAvailableTextureUnits();
        
    protected:
        GLProgram(const std::string& vertexSourceName, const std::string& fragmentSourceName, const std::string& geometrySourceName);
        
        const GLUniform& uniformByName(const std::string& name);
        void setUniformTexture(const std::string& uniformName, const GLTexture& texture);
        
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
