//
//  GLSLSMAABlendingWeightCalculation.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.10.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLSMAABlendingWeightCalculation_hpp
#define GLSLSMAABlendingWeightCalculation_hpp

#include "GLProgram.hpp"
#include "GLTexture2D.hpp"

namespace EARenderer {

    class GLSLSMAABlendingWeightCalculation : public GLProgram {
    public:
        GLSLSMAABlendingWeightCalculation();

        void setEdgesTexture(const GLFloatTexture2D<GLTexture::Float::RG16F> &edges);

        void setAreaTexture(const GLNormalizedTexture2D<GLTexture::Normalized::RG> &area);

        void setSearchTexture(const GLNormalizedTexture2D<GLTexture::Normalized::R> &search);
    };

}

#endif /* GLSLSMAABlendingWeightCalculation_hpp */
