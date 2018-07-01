//
//  GLSLGBuffer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLGBuffer_hpp
#define GLSLGBuffer_hpp

#include "GLProgram.hpp"
#include "PBRMaterial.hpp"
#include "Camera.hpp"
#include "RenderingSettings.hpp"

namespace EARenderer {

    class GLSLGBuffer: public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLGBuffer();

        void setCamera(const Camera& camera);
        void setModelMatrix(const glm::mat4& matrix);
        void setMaterial(const PBRMaterial& material);
    };

}

#endif /* GLSLGBuffer_hpp */
