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
#include "GLLDRTexture2D.hpp"
#include "GLHDRTexture2D.hpp"

namespace EARenderer {

    class GLSLSurfelClusterAveraging: public GLProgram {
    public:
        GLSLSurfelClusterAveraging();

        void setSurfelClustersGBuffer(const GLLDRTexture2D& gBuffer);
        void setSurfelsLuminaceMap(const GLHDRTexture2D& gBuffer);
    };

}

#endif /* GLSLSurfelClusterLuminanceAveraging_hpp */
