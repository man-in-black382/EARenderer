//
//  GLProgram.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLProgram_hpp
#define GLProgram_hpp

#include "GLNamedObject.hpp"

#include <map>
#include <string>

#include "GLShader.hpp"

namespace EARenderer {
    
    class GLProgram: public GLNamedObject {
    private:
        GLShader mVertexShader;
        GLShader mFragmentShader;
        std::map<std::string, int32_t> mUniforms;
        
        void link();
        void obtainUniforms();
        
    public:
        GLProgram(const GLShader&, const GLShader&);
        GLProgram(const GLProgram&) = default;
        GLProgram(GLProgram&&) = default;
        GLProgram& operator=(GLProgram);
        ~GLProgram();
        void swap(GLProgram&);
    };
    
    void swap(GLProgram&, GLProgram&);

}

#endif /* GLProgram_hpp */
