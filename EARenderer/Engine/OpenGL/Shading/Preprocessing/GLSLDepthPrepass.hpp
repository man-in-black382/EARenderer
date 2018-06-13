//
//  GLSLZPrepass.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/6/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLZPrepass_hpp
#define GLSLZPrepass_hpp

#include "GLProgram.hpp"
#include "Camera.hpp"

#include <glm/mat4x4.hpp>

namespace EARenderer {

    class GLSLDepthPrepass: public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLDepthPrepass();

        void setCamera(const Camera& camera);
        void setModelMatrix(const glm::mat4& matrix);

    };

}


#endif /* GLSLZPrepass_hpp */
