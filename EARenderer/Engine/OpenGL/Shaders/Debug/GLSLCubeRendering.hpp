//
//  GLSLCubeRendering.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 08.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLCubeRendering_hpp
#define GLSLCubeRendering_hpp

#include "GLProgram.hpp"
#include "Color.hpp"

namespace EARenderer {

    class GLSLCubeRendering: public GLProgram {
    public:
        enum class Mode { Sides, Edges };

        GLSLCubeRendering(Mode mode);

        void setViewProjectionMatrix(const glm::mat4& mvp);
        void setColor(const Color& color);
    };

}

#endif /* GLSLCubeRendering_hpp */
