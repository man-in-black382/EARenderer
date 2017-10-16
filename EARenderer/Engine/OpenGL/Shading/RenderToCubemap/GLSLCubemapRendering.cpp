//
//  GLSLCubemapRendering.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLCubemapRendering.hpp"

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace EARenderer {
    
    GLSLCubemapRendering::GLSLCubemapRendering(const std::string& fragmentShaderName)
    :
    GLProgram("CubemapRendering.vert", fragmentShaderName, "CubemapRendering.geom")
    {
        // Cubemap's coordinate system was taken from RenderMan by Pixar
        // because of that several adjustments are needed to match a conventional OpenGL coordinate system
        // Following matrices are designed to do so
        //
        // Some details:
        // https://stackoverflow.com/questions/33769539/what-is-the-reason-for-opengl-rotated-textures-when-cube-mapping
        
        glm::mat4 projMat = glm::mat4(1.0);
        projMat[2][2] = -1;
        
        glm::vec3 XNegative = { -1.0, 0.0, 0.0 };
        glm::vec3 XPositive = { 1.0, 0.0, 0.0 };
        glm::vec3 YNegative = { 0.0, -1.0, 0.0 };
        glm::vec3 YPositive = { 0.0, 1.0, 0.0 };
        glm::vec3 ZNegative = { 0.0, 0.0, -1.0 };
        glm::vec3 ZPositive = { 0.0, 0.0, 1.0 };
        
        glm::vec3 zero = glm::zero<glm::vec3>();
        
        std::array<glm::mat4, 6> matrices = {
            projMat * glm::lookAt(zero, XNegative, glm::vec3(0.0, -1.0, 0.0)),
            projMat * glm::lookAt(zero, XPositive, glm::vec3(0.0, -1.0, 0.0)),
            projMat * glm::lookAt(zero, YPositive, glm::vec3(0.0, 0.0, 1.0)),
            projMat * glm::lookAt(zero, YNegative, glm::vec3(0.0, 0.0, -1.0)),
            projMat * glm::lookAt(zero, ZPositive, glm::vec3(0.0, -1.0, 0.0)),
            projMat * glm::lookAt(zero, ZNegative, glm::vec3(0.0, -1.0, 0.0))
        };
        
        bind();
        glUniformMatrix4fv(uniformByName("uViewProjectionMatrices[0]").location(), 6, GL_FALSE, reinterpret_cast<const GLfloat *>(matrices.data()));
    }
    
    GLSLCubemapRendering::~GLSLCubemapRendering() { }
    
}
