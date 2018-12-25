//
//  GLSLBRDFIntegration.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 15.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLBRDFIntegration.hpp"

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLBRDFIntegration::GLSLBRDFIntegration()
            : GLProgram("FullScreenQuad.vert", "BRDFIntegration.frag", "") {
    }

}
