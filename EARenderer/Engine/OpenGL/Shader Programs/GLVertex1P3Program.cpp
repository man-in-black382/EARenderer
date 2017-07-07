//
//  GLNormalVisualizationProgram.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLVertex1P3Program.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {
    
    void GLVertex1P3Program::setModelViewProjectionMatrix(const glm::mat4 &mvp) {
        glUniformMatrix4fv(uniformLocation("uModelViewProjection"), 1, GL_FALSE, glm::value_ptr(mvp));
    }
    
    void GLVertex1P3Program::setColor(const glm::vec4& color) {
        glUniform4fv(uniformLocation("uColor"), 1, glm::value_ptr(color));
    }
    
    void GLVertex1P3Program::setHighlightColor(const glm::vec4& color) {
        glUniform4fv(uniformLocation("uHighlightColor"), 1, glm::value_ptr(color));
    }
    
}
