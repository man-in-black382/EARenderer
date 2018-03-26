//
//  GLSLGridLightProbesUpdate.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 3/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLGridLightProbesUpdate_hpp
#define GLSLGridLightProbesUpdate_hpp

#include "GLProgram.hpp"
#include "GLTextureBuffer.hpp"

namespace EARenderer {

    class GLSLGridLightProbesUpdate: public GLProgram {
    public:
        GLSLGridLightProbesUpdate();

//        void setSurfelClustersGBuffer(const GLLDRTexture2D& gBuffer);
//        void setSurfelsLuminaceMap(const GLHDRTexture2D& gBuffer);
    };

}


#endif /* GLSLGridLightProbesUpdate_hpp */
