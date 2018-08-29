//
//  GLSLTriangleRendering.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.12.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLSLSurfelRendering_hpp
#define GLSLSurfelRendering_hpp

#include "GLSLGenericGeometry.hpp"
#include "Color.hpp"

namespace EARenderer {
    
    class GLSLSurfelRendering: public GLProgram {
    public:
        GLSLSurfelRendering();

        void setViewProjectionMatrix(const glm::mat4& mvp);
        void setSurfelRadius(float radius);
        void setShouldUseExternalColor(bool useExternalColor);
        void setExternalColor(const Color& externalColor);
    };
    
}

#endif /* GLSLSurfelRendering_hpp */
