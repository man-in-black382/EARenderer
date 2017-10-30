//
//  GLSLEquirectangularMapConversion.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLEquirectangularMapConversion.hpp"

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLSLEquirectangularMapConversion::GLSLEquirectangularMapConversion()
    :
    GLSLCubemapRendering("EquirectangularMapConversion.frag")
    { }
    
#pragma mark - Setters

    void GLSLEquirectangularMapConversion::setEquirectangularEnvironmentMap(const GLHDRTexture2D& map) {
        setUniformTexture(ctcrc32("uEquirectangularMap"), map);
    }
    
}
