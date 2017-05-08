//
//  GLProgram.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLProgram_hpp
#define GLProgram_hpp

#include <map>
#include <string>
#include <vector>

#include "GLNamedObject.hpp"
#include "GLShader.hpp"
#include "GLBindable.hpp"
#include "GLTexture2D.hpp"
#include "GLTextureCubemap.hpp"
#include "GLDepthTexture2D.hpp"

namespace EARenderer {
    
    class GLProgram: public GLNamedObject, public GLBindable {
    private:
        const GLShader* mVertexShader;
        const GLShader* mFragmentShader;
        std::map<std::string, GLint> mUniforms;
        GLint mAvailableTextureUnits = 0;
        GLint mFreeTextureUnitIndex = 0;
        
        void link();
        void obtainUniforms();
        void obtainAvailableTextureUnits();
        void setUniformTexture(const std::string& uniformName, const GLBindable& texture);
        
    protected:
        GLint uniformLocation(const std::string& name);
        void setUniformTexture(const std::string& uniformName, const GLTexture2D& texture);
        void setUniformTexture(const std::string& uniformName, const GLTextureCubemap& texture);
        void setUniformTexture(const std::string& uniformName, const GLDepthTexture2D& texture);
        
    public:
        GLProgram(const GLShader* vertexShader, const GLShader* fragmentShader);
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
