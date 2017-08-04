//
//  GLNormalVisualizationProgram.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLGenericGeometry.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLSLGenericGeometry::GLSLGenericGeometry()
    :
    GLProgram("GenericGeometry.vert", "GenericGeometry.frag", "")
    { }
    
#pragma mark - Setters
    
    void GLSLGenericGeometry::setModelViewProjectionMatrix(const glm::mat4 &mvp) {
        glUniformMatrix4fv(uniformLocation("uModelViewProjection"), 1, GL_FALSE, glm::value_ptr(mvp));
    }
    
    void GLSLGenericGeometry::setColor(const Color& color) {
        glUniform4fv(uniformLocation("uColor"), 1, reinterpret_cast<const float *>(&color));
    }
    
    void GLSLGenericGeometry::setHighlightColor(const Color& color) {
        glUniform4fv(uniformLocation("uHighlightColor"), 1, reinterpret_cast<const GLfloat *>(&color));
    }
    
}
