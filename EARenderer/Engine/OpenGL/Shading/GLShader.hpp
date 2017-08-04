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
    
    class GLShader: public GLNamedObject {
    private:        
        void compile(const std::string& source);
        
    public:
        using GLNamedObject::GLNamedObject;
        
        GLShader(const std::string& sourcePath, GLenum type);
        GLShader(const GLShader&) = delete;
        GLShader& operator=(const GLShader&) = delete;
        ~GLShader();
        
        void swap(GLShader&);
    };
    
    void swap(GLShader&, GLShader&);
}

#endif /* GLShader_hpp */
