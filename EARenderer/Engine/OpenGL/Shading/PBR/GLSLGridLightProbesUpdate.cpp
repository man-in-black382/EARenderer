//
//  GLSLGridLightProbesUpdate.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 3/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLGridLightProbesUpdate.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLGridLightProbesUpdate::GLSLGridLightProbesUpdate()
    :
    GLProgram("FullScreenQuad.vert", "SurfelClusterAveraging.frag", "")
    {

    }

}
