//
//  GLSLSMAAEdgeDetection.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 16.10.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLSMAAEdgeDetection_hpp
#define GLSLSMAAEdgeDetection_hpp

#include "GLProgram.hpp"
#include "GLTexture2D.hpp"

namespace EARenderer {

    class GLSLSMAAEdgeDetection : public GLProgram {
    public:
        GLSLSMAAEdgeDetection();

        void setImage(const GLFloatTexture2D<GLTexture::Float::RGBA16F> &image);
    };

}

#endif /* GLSLSMAAEdgeDetection_hpp */
