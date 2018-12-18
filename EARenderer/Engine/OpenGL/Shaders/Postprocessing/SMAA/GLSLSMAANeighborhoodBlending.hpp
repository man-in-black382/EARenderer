//
//  GLSLSMAANeightborhoodBlending.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 22.10.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLSMAANeightborhoodBlending_hpp
#define GLSLSMAANeightborhoodBlending_hpp

#include "GLProgram.hpp"
#include "GLTexture2D.hpp"

namespace EARenderer {

    class GLSLSMAANeighborhoodBlending : public GLProgram {
    public:
        GLSLSMAANeighborhoodBlending();

        void setImage(const GLFloatTexture2D<GLTexture::Float::RGBA16F> &image);

        void setBlendingWeights(const GLFloatTexture2D<GLTexture::Float::RGBA16F> &weightsTexture);
    };

}

#endif /* GLSLSMAANeightborhoodBlending_hpp */
