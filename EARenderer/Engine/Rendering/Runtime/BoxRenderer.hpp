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
        enum class Mode { Sides, Edges, Full };

        const Camera *mCamera;
        std::vector<glm::vec3> mPoints;
        GLSLCubeRendering mBoxSidesRenderingShader;
        GLSLCubeRendering mBoxEdgesRenderingShader;
        GLVertexArray<glm::vec3> mVAO;
        Mode mRenderingMode;

    public:
        BoxRenderer(const Camera *camera, const std::vector<AxisAlignedBox3D>& boxes);
        BoxRenderer(Scene *scene);

        void render();
    };

}

#endif /* BoxRenderer_hpp */
