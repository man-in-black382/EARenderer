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
        const GLShader* mVertexShader;
        const GLShader* mFragmentShader;
        std::map<std::string, int32_t> mUniforms;
        
        void link();
        void obtainUniforms();
        
    public:
        GLProgram(const GLShader* vertexShader, const GLShader* fragmentShader);
        GLProgram(const GLProgram& rhs) = delete;
        GLProgram& operator=(const GLProgram& that) = delete;
        GLProgram(GLProgram&& that) = default;
        GLProgram& operator=(GLProgram&& that) = default;
        ~GLProgram();
        void swap(GLProgram&);
    };
    
    void swap(GLProgram&, GLProgram&);

}

#endif /* GLProgram_hpp */
