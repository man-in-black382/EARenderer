//
//  GLNormalVisualizationProgram.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLNormalVisualizationProgram_hpp
#define GLNormalVisualizationProgram_hpp

#include "GLProgram.hpp"

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace EARenderer {
    
    class GLVertex1P3Program: public GLProgram {
    public:
        using GLProgram::GLProgram;
        
        void setModelViewProjectionMatrix(const glm::mat4& mvp);
        void setColor(const glm::vec4& color);
        void setHighlightColor(const glm::vec4& color);
    };
    
}

#endif /* GLNormalVisualizationProgram_hpp */
