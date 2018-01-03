//
//  GLSLTriangleRendering.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.12.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLSurfelRendering.hpp"

namespace EARenderer {
    
    #pragma mark - Lifecycle
    
    GLSLSurfelRendering::GLSLSurfelRendering()
    :
    GLSLGenericGeometry("SurfelRendering.vert", "GenericGeometry.frag", "SurfelRendering.geom")
    { }
    
}
