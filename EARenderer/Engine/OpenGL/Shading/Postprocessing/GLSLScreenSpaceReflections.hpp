//
//  GLSLScreenSpaceReflections.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 15.07.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLScreenSpaceReflections_hpp
#define GLSLScreenSpaceReflections_hpp

#include "GLProgram.hpp"
#include "GLTexture2D.hpp"
#include "SceneGBuffer.hpp"
#include "Camera.hpp"

namespace EARenderer {

    class GLSLScreenSpaceReflections: public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLScreenSpaceReflections();

        void setCamera(const Camera& camera);
        void setGBuffer(const SceneGBuffer& GBuffer);
    };

}

#endif /* GLSLScreenSpaceReflections_hpp */
