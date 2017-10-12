//
//  GLSLOmnidirectionalDepth.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLSLOmnidirectionalDepth_hpp
#define GLSLOmnidirectionalDepth_hpp

#include "GLProgram.hpp"
#include "PointLight.hpp"

#include <glm/mat4x4.hpp>

namespace EARenderer {
    
    class GLSLOmnidirectionalDepth: public GLProgram {
        public:
        using GLProgram::GLProgram;
        
        GLSLOmnidirectionalDepth();
        
        void setModelMatrix(const glm::mat4& matrix);
        void setLight(const PointLight& light);
    };
    
}

#endif /* GLSLOmnidirectionalDepth_hpp */
