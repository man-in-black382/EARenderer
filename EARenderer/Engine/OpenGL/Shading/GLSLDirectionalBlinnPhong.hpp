//
//  GLBlinnPhongProgram.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLSLDirectionalBlinnPhong_hpp
#define GLSLDirectionalBlinnPhong_hpp

#include "GLProgram.hpp"
#include "Material.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "GLTexture2D.hpp"
#include "GLTexture2DArray.hpp"
#include "DirectionalLight.hpp"
#include "Camera.hpp"
#include "FrustumCascades.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace EARenderer {
    
    class GLSLDirectionalBlinnPhong: public GLProgram {
    public:
        using GLProgram::GLProgram;
        
        GLSLDirectionalBlinnPhong();
        
        void setCamera(const Camera& camera);
        void setModelMatrix(const glm::mat4& matrix);
        void setDirectionalLight(const DirectionalLight& light);
        void setMaterial(const Material& material);
        void setShadowCascades(const FrustumCascades& cascades);
        void setShadowMaps(const GLDepthTexture2DArray& shadowMaps);
        void setHighlightColor(const Color& color);
    };
    
}

#endif /* GLBlinnPhongProgram_hpp */
