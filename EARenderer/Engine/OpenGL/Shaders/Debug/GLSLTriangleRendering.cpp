//
//  GLSLTriangleRendering.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.12.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLTriangleRendering.hpp"

namespace EARenderer {
    
    #pragma mark - Lifecycle
    
    GLSLTriangleRendering::GLSLTriangleRendering()
    :
    GLSLGenericGeometry("GenericGeometry.vert", "GenericGeometry.frag", "TriangleRendering.geom")
    { }
    
}
