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
        Scene *mScene;
        std::vector<glm::vec3> mPoints;
        GLSLCubeRendering mBoxSidesRenderingShader;
        GLSLCubeRendering mBoxEdgesRenderingShader;
        GLVertexArray<glm::vec3> mVAO;

    public:
        BoxRenderer(Scene *scene);
        BoxRenderer(Scene *scene, const std::vector<AxisAlignedBox3D>& boxes);

        void render();
    };

}

#endif /* BoxRenderer_hpp */
