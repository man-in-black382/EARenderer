//
//  GLSLCookTorrance.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLSLCookTorrance_hpp
#define GLSLCookTorrance_hpp

#include "GLProgram.hpp"
#include "PBRMaterial.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "GLTexture2D.hpp"
#include "GLTexture2DArray.hpp"
#include "PointLight.hpp"
#include "Camera.hpp"
#include "FrustumCascades.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace EARenderer {
    
    class GLSLCookTorrance: public GLProgram {
        public:
        using GLProgram::GLProgram;
        
        GLSLCookTorrance();
        
        void setCamera(const Camera& camera);
        void setModelMatrix(const glm::mat4& matrix);
        void setLight(const PointLight& light);
        void setMaterial(const PBRMaterial& material);
    };
    
}

#endif /* GLSLCookTorrance_hpp */
