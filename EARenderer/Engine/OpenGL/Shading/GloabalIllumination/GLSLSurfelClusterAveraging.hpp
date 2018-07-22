//
//  GLSLSurfelClusterLuminanceAveraging.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLSurfelClusterLuminanceAveraging_hpp
#define GLSLSurfelClusterLuminanceAveraging_hpp

#include "GLProgram.hpp"
#include "GLTexture2D.hpp"
#include "GLHDRTexture2D.hpp"

namespace EARenderer {

    class GLSLSurfelClusterAveraging: public GLProgram {
    public:
        GLSLSurfelClusterAveraging();

        void setSurfelClustersGBuffer(const GLIntegerTexture2D<GLTexture::Integer::R32UI>& gBuffer);
        void setSurfelsLuminaceMap(const GLFloatTexture2D<GLTexture::Float::RGBA16F>& map);
    };

}

#endif /* GLSLSurfelClusterLuminanceAveraging_hpp */
