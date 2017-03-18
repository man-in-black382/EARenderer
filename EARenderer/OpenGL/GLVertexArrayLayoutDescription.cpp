//
//  GLVertexArrayLayoutDescription.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLVertexArrayLayoutDescription.hpp"
#include "Macros.h"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLVertexArrayLayoutDescription::GLVertexArrayLayoutDescription(std::vector<GLsizei> attributeSizes)
    :
    mAttributeSizes(attributeSizes)
    { }
    
#pragma mark - Accessors

    std::vector<GLsizei> GLVertexArrayLayoutDescription::getAttributeSizes() const {
        return mAttributeSizes;
    }
    
}
