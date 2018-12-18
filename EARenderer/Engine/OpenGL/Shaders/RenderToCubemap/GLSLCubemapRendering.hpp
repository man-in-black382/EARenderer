//
//  GLSLCubemapRendering.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLSLCubemapRendering_hpp
#define GLSLCubemapRendering_hpp

#include "GLProgram.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <string>

namespace EARenderer {

    class GLSLCubemapRendering : public GLProgram {
    public:
        using GLProgram::GLProgram;

        GLSLCubemapRendering(const std::string &fragmentShaderName);

        virtual ~GLSLCubemapRendering() = 0;
    };

}

#endif /* GLSLCubemapRendering_hpp */
