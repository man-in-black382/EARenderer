//
//  BoxRenderer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 08.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef BoxRenderer_hpp
#define BoxRenderer_hpp

#include "GLSLCubeRendering.hpp"
#include "GLVertexArray.hpp"
#include "AxisAlignedBox3D.hpp"
#include "Scene.hpp"

#include <vector>
#include <glm/vec3.hpp>

namespace EARenderer {

    class BoxRenderer {
    private:
        const Camera *mCamera;
        std::vector<glm::vec3> mPoints;
        GLSLCubeRendering mBoxSidesRenderingShader;
        GLSLCubeRendering mBoxEdgesRenderingShader;
        GLVertexArray<glm::vec3> mVAO;

    public:
        enum class Mode { Sides, Edges, Full };

        BoxRenderer(const Camera *camera, const std::vector<AxisAlignedBox3D>& boxes);
        BoxRenderer(Scene *scene);

        void render(Mode renderingMode, const glm::mat4& boxTransform = glm::mat4(1.0));
    };

}

#endif /* BoxRenderer_hpp */
