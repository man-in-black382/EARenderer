//
//  GLNormalVisualizationProgram.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLLineVisualizationProgram.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {
    
    void GLLineVisualizationProgram::setModelViewProjectionMatrix(const glm::mat4 &mvp) {
        glUniformMatrix4fv(uniformLocation("uModelViewProjection"), 1, GL_FALSE, glm::value_ptr(mvp));
    }
    
}
