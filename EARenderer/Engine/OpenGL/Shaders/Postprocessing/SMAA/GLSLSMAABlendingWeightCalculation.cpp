//
//  GLSLSMAABlendingWeightCalculation.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.10.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLSMAABlendingWeightCalculation.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLSMAABlendingWeightCalculation::GLSLSMAABlendingWeightCalculation()
    :
    GLProgram("SMAABlendingWeightCalculation.vert", "SMAABlendingWeightCalculation.frag", "")
    { }

#pragma mark - Setters

    void GLSLSMAABlendingWeightCalculation::setEdgesTexture(const GLFloatTexture2D<GLTexture::Float::RG16F>& edges) {
        setUniformTexture(uint32_constant<ctcrc32("uEdgesTexture")>, edges);
    }

    void GLSLSMAABlendingWeightCalculation::setAreaTexture(const GLNormalizedTexture2D<GLTexture::Normalized::RG>& area) {
          setUniformTexture(uint32_constant<ctcrc32("uAreaTexture")>, area);
    }

    void GLSLSMAABlendingWeightCalculation::setSearchTexture(const GLNormalizedTexture2D<GLTexture::Normalized::R>& search) {
        setUniformTexture(uint32_constant<ctcrc32("uSearchTexture")>, search);
    }

}
