//
//  GLNormalVisualizationProgram.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLNormalVisualizationProgram_hpp
#define GLNormalVisualizationProgram_hpp

#include "GLProgram.hpp"
#include <glm/mat4x4.hpp>

namespace EARenderer {
    
    class GLLineVisualizationProgram: public GLProgram {
    public:
        using GLProgram::GLProgram;
        
        void setModelViewProjectionMatrix(const glm::mat4& mvp);
    };
    
}

#endif /* GLNormalVisualizationProgram_hpp */
