//
//  GLVertexAttribute.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.02.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLVertexAttribute.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLVertexAttribute::GLVertexAttribute(GLint sizeInBytes, GLint componentCount)
    :
    bytes(sizeInBytes),
    components(componentCount)
    { }

    GLVertexAttribute::GLVertexAttribute(GLint sizeInBytes, GLint componentCount, GLint divisor)
    :
    bytes(sizeInBytes),
    components(componentCount),
    divisor(divisor)
    { }

    GLVertexAttribute GLVertexAttribute::UniqueAttribute(GLint sizeInBytes, GLint componentCount) {
        return GLVertexAttribute(sizeInBytes, componentCount, 0);
    }

    GLVertexAttribute GLVertexAttribute::SharedAttribute(GLint sizeInBytes, GLint componentCount) {
        return GLVertexAttribute(sizeInBytes, componentCount, 1);
    }

}
