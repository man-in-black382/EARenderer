//
//  GLSLEquirectangularMapConversion.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLEquirectangularMapConversion.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLSLEquirectangularMapConversion::GLSLEquirectangularMapConversion()
    :
    GLProgram("EquirectangularMapConversion.vert", "EquirectangularMapConversion.frag", "EquirectangularMapConversion.geom")
    { }
    
#pragma mark - Setters
    
    void GLSLEquirectangularMapConversion::setViewProjections(CommonGeometricEntities::OmnidirectionalViewProjectionMatrixSet matrices) {
        glUniformMatrix4fv(uniformByName("uViewProjectionMatrices[0]").location(), 6, GL_FALSE, reinterpret_cast<const GLfloat *>(matrices.data()));
    }
    
    void GLSLEquirectangularMapConversion::setEquirectangularEnvironmentMap(const GLHDRTexture2D& map) {
        setUniformTexture("uEquirectangularMap", map);
    }
    
}
