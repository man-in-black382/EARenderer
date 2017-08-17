//
//  GLBlinnPhongProgram.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLSLOmnidirectionalBlinnPhong_hpp
#define GLSLOmnidirectionalBlinnPhong_hpp

#include "GLProgram.hpp"
#include "Material.hpp"
#include "GLDepthTextureCubemap.hpp"
#include "PointLight.hpp"
#include "Camera.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace EARenderer {
    
    class GLSLOmnidirectionalBlinnPhong: public GLProgram {
    public:
        using GLProgram::GLProgram;
        
        GLSLOmnidirectionalBlinnPhong();
        
        void setCamera(const Camera& camera);
        void setModelMatrix(const glm::mat4& matrix);
        void setPointLight(const PointLight& light);
        void setMaterial(const Material& material);
        void setShadowMap(const GLDepthTextureCubemap& shadowMap);
        void setHighlightColor(const Color& color);
    };
    
}

#endif /* GLBlinnPhongProgram_hpp */
