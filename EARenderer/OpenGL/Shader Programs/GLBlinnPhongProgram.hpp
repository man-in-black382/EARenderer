//
//  GLBlinnPhongProgram.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLBlinnPhongProgram_hpp
#define GLBlinnPhongProgram_hpp

#include "GLProgram.hpp"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace EARenderer {

//    uniform mat4 uViewProjectionMat;
//    uniform mat4 uModelMat;
//    uniform mat4 uNormalMat;
//    
//    uniform vec3 uLightPosition;
//    uniform vec3 uCameraPosition;
    
    class GLBlinnPhongProgram: public GLProgram {
    public:
        using GLProgram::GLProgram;
        
        void setCameraPosition(const glm::vec3& position);
        void setLightPosition(const glm::vec3& position);
        void setViewProjectionMatrix(const glm::mat4& matrix);
        void setModelMatrix(const glm::mat4& matrix);
        void setNormalMatrix(const glm::mat4& matrix);
        void setLightColor(const glm::vec3& color);
        void setMaterial(const glm::vec3& ambientReflectances,
                         const glm::vec3& diffuseReflectances,
                         const glm::vec3& specularReflectances,
                         float specularExponent);
    };
    
}

#endif /* GLBlinnPhongProgram_hpp */
