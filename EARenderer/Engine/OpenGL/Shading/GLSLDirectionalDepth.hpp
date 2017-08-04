//
//  GLDepthFillerProgram.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLDepthFillerProgram_hpp
#define GLDepthFillerProgram_hpp

#include "GLProgram.hpp"

#include <glm/mat4x4.hpp>

namespace EARenderer {
    
    class GLSLDirectionalDepth: public GLProgram {
    public:
        using GLProgram::GLProgram;
        
        GLSLDirectionalDepth();
        
        void setModelMatrix(const glm::mat4& matrix);
        void setViewProjectionMatrix(const glm::mat4& matrix);
    };
    
}

#endif /* GLDepthFillerProgram_hpp */
