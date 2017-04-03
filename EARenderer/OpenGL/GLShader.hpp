//
//  GLShader.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLShader_hpp
#define GLShader_hpp

#include <string>

#include "GLNamedObject.hpp"

namespace EARenderer {
    
    enum class GLShaderType { vertex, fragment };
    
    class GLShader: public GLNamedObject {
    private:
        std::string mSource;
        
        void compile();
        
    public:
        GLShader(const std::string& sourcePath, GLenum type);
        GLShader(const GLShader&) = delete;
        GLShader& operator=(const GLShader&) = delete;
        GLShader(GLShader&&) = default;
        GLShader& operator=(GLShader&&) = default;
        ~GLShader();
        void swap(GLShader&);
        
        const std::string& source();
    };
    
    void swap(GLShader&, GLShader&);
}

#endif /* GLShader_hpp */
