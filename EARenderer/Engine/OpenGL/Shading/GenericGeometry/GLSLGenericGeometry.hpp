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

#include "Color.hpp"

namespace EARenderer {
    
    class GLSLGenericGeometry: public GLProgram {
    public:
        using GLProgram::GLProgram;
        
        GLSLGenericGeometry();
        
        void setModelViewProjectionMatrix(const glm::mat4& mvp);
        void setColor(const Color& color);
        void setHighlightColor(const Color& color);
    };
    
}

#endif /* GLNormalVisualizationProgram_hpp */
